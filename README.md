picotls
===

[![Build Status](https://travis-ci.org/h2o/picotls.svg?branch=master)](https://travis-ci.org/h2o/picotls)

Picotls is a [TLS 1.3 (RFC 8446)](https://tools.ietf.org/html/rfc8446) implementation written in C, with the following features:
* support for two crypto engines
  * "OpenSSL" backend using libcrypto for crypto and X.509 operations
  * "minicrypto" backend using [cifra](https://github.com/ctz/cifra) for most crypto and [micro-ecc](https://github.com/kmackay/micro-ecc) for secp256r1
* support for PSK, PSK-DHE resumption using 0-RTT
* API for dealing directly with TLS handshake messages (essential for QUIC)

Primary goal of the project is to create a fast, tiny TLS 1.3 implementation that can be used with the HTTP/2 protocol stack and possibly the upcoming QUIC stack of the [H2O HTTP/2 server](https://h2o.examp1e.net).

The TLS protocol ipmlementation of picotls is licensed under the MIT license.

License and the cryptographic algorithms supported by the crypto bindings are as follows:

| Binding | License | Key Exchange | Certificate | AEAD cipher |
|:-----:|:-----:|:-----:|:-----:|:-----:|
| minicrypto | [CC0](https://github.com/ctz/cifra/) / [2-clause BSD](https://github.com/kmackay/micro-ecc) | secp256r1, x25519 | ECDSA (P256)<sup>1</sup> | AES-128-GCM, chacha20-poly1305 |
| OpenSSL | OpenSSL | secp256r1, secp384r1, secp521r1, x25519 | RSA, ECDSA (P256) | AES-128-GCM, chacha20-poly1305 |

Note 1: Minicrypto binding is capable of signing a handshake using the certificate's key, but cannot verify a signature sent by the peer.

Building picotls
---

If you have cloned picotls from git then ensure that you have initialised the submodules:
```
% git submodule init
% git submodule update
```

Build using cmake:
```
% cmake .
% make
% make check
```

A dedicated documentation for using picotls with Visual Studio can be found in [WindowsPort.md](WindowsPort.md).

Building details
---

Following the instructions above, cmake will search for OpenSSL.  If OpenSSL is installed in a non-standard location or you wish to use a specific version of it, you have specify the location for the root directory on the `cmake` command line:

```
cmake -DOPENSSL_ROOT_DIR=/openssl/root/directory
```
For example:
```
cmake -DOPENSSL_ROOT_DIR=/home/user/proj2/openssl
```

Similarly, you can force the use of static libraries for OpenSSL:

```
cmake -DOPENSSL_USE_STATIC_LIBS=TRUE
```

Compiling with BoringSSL
---

The picotls can now use the BoringSSL library from Google as a replacement for OpenSSL.  Again, this is defined with parameters to cmake:
```
cmake -DBORING_ROOT=/boring/root/directory -DBORING_LIBRARY_DIR=/boring/root/lib
```
For example:
```
cmake -DBORING_ROOT=/home/user/proj/thirdparty/boringssl -DBORING_LIBRARY_DIR=/home/user/proj/thirdparty/lib
```

Brotli
---

Picotls will search for Brotli libraries and use them if found.  However, you can specify their location on the command line to cmake:

```
cmake -DBROTLI_ROOT=/directory/to/brotli -DBROTLI_DEC=/full/path/to/decryption/library.a -DBROTLI_ENC=/full/path/to/encryption/library.a
```

Developer documentation
---

Developer documentation should be available on [the wiki](https://github.com/h2o/picotls/wiki).


Private key methods
---

The programming interface for picotls remains the same for BoringSSL as for OpenSSL.  But there is a feature of BoringSSL which can improve performance of your application: private_key_method.  Within BoringSSL you can specify a set of callback functions to be performed during the handshake for signing a certificate and for decryption.  The performance benefit is gained by off-loading these tasks to a separate processor, thread or task, which frees the main thread of your process to continue to be highly performant.

There is little to be gained in performance in decryption so that particular callback is not implemented; however the callback to implement signing a certificate is.  This is a server only funcation so it's not useful if you are implementing client only functions.

There is a single structure `ptls_private_key_method_t` with two callback functions you specify: `sign` and `complete`.

To enable the feature, create a context as you normally do (ptls_context_t).  In the context, there is a member: `private_key_method` which is of type: `ptls_private_key_method_t *`.  Assign it's value to a static which points the to `sign` and `complete` functions in your program.

The `sign` function:
```
    enum ptls_private_key_result_t (*sign)(ptls_sign_certificate_t *do_sign,
                                           ptls_t *tls,
                                           uint16_t *selected_algorithm,
                                           ptls_buffer_t *output,
                                           ptls_iovec_t input,
                                           const uint16_t *algorithms,
                                           size_t num_algorithms);

```
Inputs:
   - do_sign: The system sign function which can be called in a separate process or thread using the remaining parameters:
   - tls: The active session.
   - input: A buffer and length of the input to be used.  You must make a copy of this as it is only passed once.
   - algorithms: An array of algoriths which are legal for this function.  You must make a copy of this as it is only passed once.
   - num_algorithms:  The number of algorithms in the algorithms array.

Outputs:

   - selected_algorithm: One of the algorithms passed in, the one use for the signature.
   - output: The length and data of the signature.

Returns: one of the enumerated values: 
   - `ptls_private_key_success`: Indicates that the data has been processed into output and selected_algorithm.
   - `ptls_private_key_retry`: The process is still running.  The complete function will be called repeatedly until a return code other than this is returned.
   - `ptls_private_key_failure`: Indicates that the operation failed.

Most likely your initial `sign` function will return `ptls_private_key_retry` so picotls will need to poll your application until it indicates it is done.  It does this with the `complete` function:

```
    enum ptls_private_key_result_t (*complete)(ptls_sign_certificate_t *do_sign,
                                               ptls_t *tls,
                                               uint16_t *selected_algorithm,
                                               ptls_buffer_t *output);

```
Parameters are the same as for `sign`.

Using the cli command
---

Run the test server (at 127.0.0.1:8443):
```
% ./cli -c /path/to/certificate.pem -k /path/to/private-key.pem  127.0.0.1 8443
```

Connect to the test server:
```
% ./cli 127.0.0.1 8443
```

Using resumption:
```
% ./cli -s session-file 127.0.0.1 8443
```
The session-file is read-write.
The cli server implements a single-entry session cache.
The cli server sends NewSessionTicket when it first sends application data after receiving ClientFinished.

Using early-data:
```
% ./cli -s session-file -e 127.0.0.1 8443
```
When `-e` option is used, client first waits for user input, and then sends CLIENT_HELLO along with the early-data.

License
---

The software is provided under the MIT license.
Note that additional licences apply if you use the minicrypto binding (see above).
