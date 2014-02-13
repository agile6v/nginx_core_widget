
/*
 * Copyright (C) agile6v
 */

#include <ngx_config.h>
#include <ngx_core.h>
#include <nginx.h>
#include <ngx_http.h>

typedef struct {
    ngx_array_t         *types_keys;
    ngx_hash_t           types;
} ngx_http_widget_loc_conf_t;

static ngx_int_t ngx_http_widget_add_variables(ngx_conf_t *cf);
static void *ngx_http_widget_create_loc_conf(ngx_conf_t *cf);
static char *ngx_http_widget_merge_loc_conf(ngx_conf_t *cf, void *parent, void *child);
static ngx_int_t ngx_http_widget_request_no_cacheable_variable(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);

static ngx_str_t ngx_http_widget_no_cacheable_var = ngx_string("no_cacheable");
    
static ngx_command_t  ngx_http_widget_commands[] = {
    
    { ngx_string("cache_types"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_1MORE,
      ngx_http_types_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_widget_loc_conf_t, types_keys),
      NULL },
      
      ngx_null_command
};


ngx_http_module_t  ngx_http_widget_module_ctx = {
    ngx_http_widget_add_variables,          /* preconfiguration */
    NULL,                                   /* postconfiguration */
    NULL,                                   /* create main configuration */
    NULL,                                   /* init main configuration */
    NULL,                                   /* create server configuration */
    NULL,                                   /* merge server configuration */
    ngx_http_widget_create_loc_conf,        /* create location configuration */
    ngx_http_widget_merge_loc_conf          /* merge location configuration */
};

ngx_module_t  ngx_http_widget_module = {
    NGX_MODULE_V1,
    &ngx_http_widget_module_ctx,      /* module context */
    ngx_http_widget_commands,         /* module directives */
    NGX_HTTP_MODULE,                  /* module type */
    NULL,                             /* init master */
    NULL,                             /* init module */
    NULL,                             /* init process */
    NULL,                             /* init thread */
    NULL,                             /* exit thread */
    NULL,                             /* exit process */
    NULL,                             /* exit master */
    NGX_MODULE_V1_PADDING
};

static void *
ngx_http_widget_create_loc_conf(ngx_conf_t *cf)
{
    ngx_http_widget_loc_conf_t  *conf;

    conf = ngx_pcalloc(cf->pool, sizeof(ngx_http_widget_loc_conf_t));
    if (conf == NULL) {
        return NULL;
    }
    
    return conf;
}

static char *
ngx_http_widget_merge_loc_conf(ngx_conf_t *cf, void *parent, void *child)
{
    ngx_http_widget_loc_conf_t *prev = parent;
    ngx_http_widget_loc_conf_t *conf = child;
    
    if (ngx_http_merge_types(cf, &conf->types_keys, &conf->types,
                             &prev->types_keys, &prev->types,
                             ngx_http_html_default_types)
        != NGX_OK)
    {
        return NGX_CONF_ERROR;
    }
    
    return NGX_CONF_OK;
}

static ngx_int_t 
ngx_http_widget_add_variables(ngx_conf_t *cf)
{
    ngx_http_variable_t     *var;

    var = ngx_http_add_variable(cf, &ngx_http_widget_no_cacheable_var, NGX_HTTP_VAR_NOHASH);
    if (var == NULL) {
        return NGX_ERROR;
    }

    var->get_handler = ngx_http_widget_request_no_cacheable_variable;
   
    return NGX_OK;
}

static ngx_int_t 
ngx_http_widget_request_no_cacheable_variable(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data)
{
    ngx_http_widget_loc_conf_t  *wlcf;

    wlcf = ngx_http_get_module_loc_conf(r, ngx_http_widget_module);

    if (ngx_http_test_content_type(r, &wlcf->types) == NULL) {
        *v = ngx_http_variable_true_value;
        return NGX_OK;
    }
    
    *v = ngx_http_variable_null_value;
    return NGX_OK;
}

