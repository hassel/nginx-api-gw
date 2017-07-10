proxy_set_header Proxy-IP           $remote_addr;
proxy_set_header Proxy-Keysize      512;
add_header Strict-Transport-Security "max-age=31536000; includeSubdomains; preload";
add_header X-Content-Type-Options nosniff;