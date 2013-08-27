/*
 * Copyright (C) agile6v
 */

#include <ngx_config.h>
#include <ngx_core.h>
#include <nginx.h>


static void *ngx_core_widget_create_conf(ngx_cycle_t *cycle);
static char *ngx_core_widget_module_init_conf(ngx_cycle_t *cycle, void *conf);

typedef struct {
    ngx_flag_t  cpu_affinity_auto;
} ngx_core_widget_conf_t;


static ngx_command_t  ngx_core_widget_commands[] = {

    { ngx_string("worker_cpu_affinity_auto"),
      NGX_MAIN_CONF|NGX_DIRECT_CONF|NGX_CONF_FLAG,
      ngx_conf_set_flag_slot,
      0,
      offsetof(ngx_core_widget_conf_t, cpu_affinity_auto),
      NULL },

    ngx_null_command
};

static ngx_core_module_t  ngx_core_widget_module_ctx = {
    ngx_string("core_widget"),
    ngx_core_widget_create_conf,
    ngx_core_widget_module_init_conf
};


ngx_module_t  ngx_core_widget_module = {
    NGX_MODULE_V1,
    &ngx_core_widget_module_ctx,           /* module context */
    ngx_core_widget_commands,              /* module directives */
    NGX_CORE_MODULE,                       /* module type */
    NULL,                                  /* init master */
    NULL,                                  /* init module */
    NULL,                                  /* init process */
    NULL,                                  /* init thread */
    NULL,                                  /* exit thread */
    NULL,                                  /* exit process */
    NULL,                                  /* exit master */
    NGX_MODULE_V1_PADDING
};


static void *
ngx_core_widget_create_conf(ngx_cycle_t *cycle)
{
    ngx_core_widget_conf_t  *cwcf;

    cwcf = ngx_pcalloc(cycle->pool, sizeof(ngx_core_widget_conf_t));
    if (cwcf == NULL) {
        return NULL;
    }

    cwcf->cpu_affinity_auto = NGX_CONF_UNSET;
     
    return cwcf;
}

static char *
ngx_core_widget_module_init_conf(ngx_cycle_t *cycle, void *conf)
{

#if (NGX_HAVE_CPU_AFFINITY)
    ngx_core_widget_conf_t  *cwcf = conf;
    ngx_core_conf_t          *ccf;
    uint64_t                 *mask;
    ngx_int_t                 n;

    ccf = (ngx_core_conf_t *) ngx_get_conf(cycle->conf_ctx,
                                           ngx_core_module);
                                           
    ngx_conf_init_value(cwcf->cpu_affinity_auto, 0);
    
    if (cwcf->cpu_affinity_auto 
        && ccf->worker_processes > 1
        && ngx_ncpu > 1)
    {
        //  check whether directive "worker_cpu_affinity" already set.
        if (ccf->cpu_affinity_n == 0 && ccf->cpu_affinity == NULL) {
        
            mask = ngx_palloc(cycle->pool, ngx_ncpu * sizeof(uint64_t));
            if (mask == NULL) {
                return NGX_CONF_ERROR;
            }
     
            ccf->cpu_affinity = mask;
            ccf->cpu_affinity_n = ngx_ncpu;
            
            for (n = 0; n < ngx_ncpu; n++) {

                mask[n] = 0;
                
                mask[n] = 1 << n;
            }
            
            //  check whether the worker number exceeds the number of cpu cores
            if (ccf->cpu_affinity_n != (ngx_uint_t) ccf->worker_processes) {
                ngx_log_error(NGX_LOG_WARN, cycle->log, 0,
                              "the number of \"worker_processes\" is not equal to "
                              "the number of \"cpu\" "
                              "using last cpu for remaining worker processes");
            }
        } else {
            ngx_log_error(NGX_LOG_WARN, cycle->log, 0,
                          "the directive \"worker_cpu_affinity\" already set and "
                          "the directive \"worker_cpu_affinity_auto\" will not work!");
        }
    }
    
#endif

    return NGX_CONF_OK;
}

