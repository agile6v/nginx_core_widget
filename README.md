Introduction
====
This module is collection of small and useful nginx add-ons.


Installation
====
`./configure --add-module=/path/to/nginx_core_widget`   



Directives
====

worker_cpu_affinity_auto
--------------------
**syntax:** *worker_cpu_affinity_auto on|off*

**default:** *worker_cpu_affinity_auto off*

**context:** *main*

This directive makes the worker process according to the number of CPU automatically bound to the CPU.


cache_types
--------------------
**syntax:** *cache_types mime-type ...*

**default:** *cache_types text/html;*

**context:** *http, server, location*

Defines the MIME type of a response can be cached. The special value “*” matches any MIME type. Default, responses with the “text/html” type are always cached.

``` 
http {

    cache_types text/html text/css image/gif;
    
    proxy_cache_path /data/cache/ keys_zone=tmpcache:100m max_size=1g;

    server {
        
        location /cache {
            proxy_no_cache $no_cacheable;
            proxy_cache tmpcache;
            proxy_pass http://127.0.0.1:25000;
        }    
    }

}

```


Variables
====
$no_cacheable
--------------------
    The variable need to be used with cache_types and proxy_no_cache. 
    If the MIME type of the response in "cache_types" directive, this varibale will return 1, otherwise it returns 0.



