# LB model for nginx
![N|Solid](https://www.nginx.com/wp-content/themes/nginx-theme/assets/img/nginx-logos/nplus-119x119.png) 


> This is a group-varliable yaml
> it should be located in /inventory/group_vars/group-name
 
## Vars, defaults originated from the role nginx.

 Example:

```yaml
 site: sth                                          Site names, used for logging. Has no default
 machinetype: lb                                    Machine type. No implemented usage. Has no default
 ciphers: "EECDH+AESGCM:EDH+AESGCM:..."             "ciphers:" values are originated from ngnix/vars

 nginx_version: 1.9.13-2                            "nginx_version:" value originates from nginx/defaults, sets the nginx-plus version to be installed

 header_files:                                      "header_files" values are originated form nginx/defaults
  - header_http.inc                                 Defineds vip-global header injection for all vips in a cluster. GFS headers are normally only used internally when glass-fish is used.
  - header_gsf_http.inc                             header_gfs_* and header_* can NOT and SHOULD not be defined at the same time.
  - header_https.inc                                *.enable files are used for location specific header injection.
  - header_gsf_https.inc
  - ws.enbale
  - gf.enable

 monitor_addr:                                      "monitor_addr": should originate from cluster-id group_vars
  - 10.35.15.222                                    Defined where to publish nginx status api, other then 127.0.0.1
  - 10.35.15.223
  - 10.35.15.224

 api_acl:                                           "api_acl" values are originated from nginx/vars
   - 10.1.48.0/20                                   Defines which cird that is allowed to use the upstream_conf api

 monitor_acl:                                       "monitor_acl" values are originated from nginx/vars
  - 10.1.48.0/20                                    Defines which cidr that is allowed to reach the status api and dashboard
  - 10.1.32.0/24

 probes:                                            "probes" values are originated from nginx/vars
   monitoring-foo:                                  Name of the probe/health check
     status: 200                                    Expected status code
     header: Content-Type = application/json        Expected Content-type
     body: '!~ "varnish"'                           Make sure "varnish" is not a part of the body.
```

## Config generation structuure
 * vips: list of vips to be setup. key will be the name of the config file name. Values originated from group_var
 * upstreams: list of avalible upstreams to be used in a vip. key will be name of the config file name. Values originated from group_var
 * probes: list of probes to be used in a vip. key will be a part of the config file name.
 
## Eaxmple:

```yaml
 vips: 
   key1:
   key2:

 upstreams:
   key1:
   key2:

 probes:
   key1:
   key2:
```
##  VIP variabels

### Example 

```yaml
vips:
   vip_name: 
      server_name: example.meh.com                SNI Server-name
      server_name:
        - can.be.a.list.a.meh.com                 This can also be a list
        - can.be.a.list.b.meh.com                 
      listen: listen-addr:port                      Only define port when L4 vip, L7 vips (http/https) only requires an address
      listen:
        - listen-addr1:port1                        This can also be a list
        - listen-addr2:port2
      proto:                                        If tcp or udp is defined, no other protocols can be defined. If http|https is defined, https|http can also be defined.
        - https                                     TLS enabled http vip
        - http                                      http enabled vip
        - wss                                       WebSocketSecure enabled vip without no L7 filter (only serves /). 
        - tcp                                       tcp enabled vip
	- udp					    udp enabled vip
      prefix: conf|stream                           Config prefix, determins where in the core config should be included. Prefix "config" is used for L7 vips, "stream" for L4 vips.
      ssl_crt: /etc/ssl/cert.pem                    Certificate in pem format.
      ssl_key: /etc/ssl/key.pem                     Key in pem format.
      ssl_dhparam:                                  Will override default to dhparams.pem, can be set to dhparams1024.pem for legacy applications.
      ciphers: "EECDH+AESGCM:EDH+AESGCM:..."        Will override default ciphers.
      ssl_staple: /etc/ssl/gd_bundle.crt            When defined, oscp will be enabled. Note that external resolving & http reqs needs to be available for this to work. If oscp stapling is not wanted, leave the var undefined.
      rate_limit: 600r/m                            Rate metrics for "vip-global" rate-limiting. Format, nr/t. Quote "The rate is specified in requests per second (r/s). If a rate of less than one request per second is desired, it is specified in request per minute (r/m). For example, half-request per second is 30r/m."
      rate_limit_delay: no
      send_timeout: 5s                              Defines a timeout for sending a request to the proxied servers for this vip
      read_timeout: 5s                              Defines a timeout for reading a response from the proxied servers for this vip
```

## Location/Context variables

### Example 

```yaml
    locations:                                      List of locations, location_1 location_2, app_1 app_2 etc.
      location_1:                                   Readable comment, needs to be uniq
        probe: monitoring-default                   Name of the probe to be configured for these locations, needs to match probes available from nginx/vars/main.yml (monitoring-json, monitoring-xml, monitoring-200, monitoring-default)
        probe_port: 6081                            Port to probe, default port if blank, probe needs to be defined
        probe_uri: /bar/json                        Uri for the health check to probe for response/data, probe needs to be defined
        probe_interval: 5                           Interval, default 10 if blank, probe needs to be defined
        probe_fails: 5                              Number of fails before unavail, default 2, probe needs to be defined
        proxy_buff: "off"                           Set when there is a need to disable proxy_buffering, sse & websocket etc. on/off (default on)
        allow_upgrade: "true"                       Allow connection to be upgraded to websocket for this specific location. "true"/"fales" (default false)
        legacy_glassfish: "true"                    Inject headers needed for legacy glassfish domain for this specific location. "true"/"false" (default false)
        send_timeout: 5s                            Defines a timeout for sending a request to the proxied servers for this location
        read_timeout: 5s                            Defines a timeout for reading a response from the proxied servers for this location
        context: /foo.xml                           Context-path to be accessed (recursive)
        backend: sfarm-foo                          Name of the backend-group configured to serve the specific location, needs to match with the configured upstream. 
      location_2:                                   If you're using multiple contexts to the same backend, make sure to use a list, else health-check logic wont work as expected
        context: 
          - /bar/                                   -||-
          - /baz/                                   -||-
        backend: sfarm-bar                          -||-
```
## Upstream variables 

### Example
```yaml
 upstreams: 
   upstream_name:                                   Name of the backend-group
    prefix: sfarm|tcp                               Config prefix, determins where in the core config it should be included. Prefix "sfarm" is used for L7 vips, "tcp" for L4 vips.
    backends:
      - backendaddr:port1                           backend ipv4 address & port. This is handles as a string by ansible.
      - backendaddr:port2                           -||-
      - backendaddr:port3 down                      -||-
    algo: least_conn                                Algorithm for balance decisions, more [info](http://nginx.org/en/docs/http/ngx_http_upstream_module.html) here and in addition "sticky_cookie" to enable cookie injection. 
    fail_time: 20                                   Passive servercheck metrics  
    max_fail: 10                                    -||-
```

###   Example L4 vip
```yaml
 vips:
   vipname:
     server_name: offering-mq-.bru.service.meh.com
     listen: 10.10.10.10:61616
     proto:
      - tcp
     prefix: stream
     backend: sfarm-offering-mq

   sfarm-offering-mq:
     in_catalog: no
     prefix: tcp
     backends:
       - 10.10.10.11:61616
       - 10.10.10.12:61616
     algo: least_conn
```
###  Example L7 vip 
```yaml
 vips:
   api: 
      server_name: e4-api.meh.com
      listen: 185.63.76.8
      proto: 
        - https
        - http
      prefix: conf
      ssl_crt: /etc/ssl/wildcard_meh_bundle.crt
      ssl_key: /etc/ssl/wildcard_meh_bundle.key
      ssl_staple: /etc/ssl/gd_bundle.crt
      rate_limit: 600r/m
      rate_limit_delay: no
    locations:
      location_1:
        context: /crossdomain.xml
        backend: sfarm-offering
        probe: monitoring-xml
        probe_port: 6081
        probe_uri: /crossdomain.xml
        probe_interval: 10
        probe_fails: 5
      location_2:
        context: /offering/
        backend: sfarm-offering

 upstreams:
   sfarm-offering:
     in_catalog: no
     prefix: sfarm
     backends:
       - 10.50.3.65:6081
       - 10.50.3.66:6081
     algo: hash $request_uri

  sfarm-static:
    in_catalog: no 
    prefix: sfarm
    backends:
      - 10.50.3.65:6081
      - 10.50.3.66:6081
    algo: hash $request_uri
 
  probes:
   monitoring-xml:
     status: 200
     header: Content-Type = application/xml
     body: '!~ "varnish"'
   monitoring-json:
     status: 200
     header: Content-Type = application/json
     body: '!~ "varnish"'
```
