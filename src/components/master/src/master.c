
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/* Files required for transport initialization. */
#include <coresrv/nk/transport-kos.h>
#include <coresrv/sl/sl_api.h>

/* EDL description of the server entity. */
#include <ucm/Master.edl.h>
#include <ucm/Master.idl.h>
#include <ucm/OTA.idl.h>

#include <assert.h>

/* Type of interface implementing object. */
typedef struct IMasterImpl {
    struct ucm_Master base;     /* Base interface of object */
    rtl_uint32_t step;         /* Extra parameters */
    struct ucm_OTA* ota;        /* OTA Client */
} IMasterImpl;


static nk_err_t CheckUpdates_OTA(struct ucm_OTA* ota, nk_uint8_t* result)
{
    ucm_OTA_CheckUpdates_req req;
    ucm_OTA_CheckUpdates_res res;

    nk_err_t err = ucm_OTA_CheckUpdates(ota, &req, NULL, &res, NULL);
    if (err == NK_EOK)
    {
        *result = res.result;
    }
    else
    {
        fprintf(stderr, "[Master] CheckUpdates (OTA) failed: %d\n", (int)err);
    }
    return err;
}

static nk_err_t CheckUpdates_impl(struct ucm_Master *self,
                          const struct ucm_Master_CheckUpdates_req *req,
                          const struct nk_arena *req_arena,
                          struct ucm_Master_CheckUpdates_res *res,
                          struct nk_arena *res_arena)
{
    IMasterImpl *impl = (IMasterImpl *)self;
    fprintf(stderr, "[Master] CheckUpdates request (OTA: %d)\n", (int)impl->ota);
    nk_uint8_t result = ucm_OTA_UpdatesUnAvailable;
    nk_err_t err = CheckUpdates_OTA(impl->ota, &result);
    if (err == NK_EOK)
    {
        res->result = result == ucm_OTA_UpdatesAvailable ? ucm_Master_UpdatesAvailable : ucm_Master_UpdatesUnAvailable;
    }
    return err;
}

static nk_err_t Update_impl(struct ucm_Master *self,
                          const struct ucm_Master_Update_req *req,
                          const struct nk_arena *req_arena,
                          struct ucm_Master_Update_res *res,
                          struct nk_arena *res_arena)
{
    IMasterImpl *impl = (IMasterImpl *)self;
    res->result = ucm_Master_UpdateSucceeded;
    return NK_EOK;
}


static struct ucm_OTA *CreateOTA_Impl()
{
    Handle handle = ServiceLocatorConnect("ota_connection");
    assert(handle != INVALID_HANDLE);
    static NkKosTransport transport;
    /* Initialize IPC transport for interaction with the server entity. */
    NkKosTransport_Init(&transport, handle, NK_NULL, 0);
    nk_iid_t riid = ServiceLocatorGetRiid(handle, "ucm.OTA.ota");
    assert(riid != INVALID_RIID);
    static struct ucm_OTA_proxy ota;
    ucm_OTA_proxy_init(&ota, &transport.base, riid);
    return &ota.base;
}


static struct ucm_Master *CreateIMasterImpl(rtl_uint32_t step)
{
    /* Table of implementations of UCM Master interface methods. */
    static const struct ucm_Master_ops ops = {
        .CheckUpdates = CheckUpdates_impl,
        .Update = Update_impl
    };

    /* Interface implementing object. */
    static struct IMasterImpl impl = {
        .base = {&ops}
    };

    impl.ota = CreateOTA_Impl();
    impl.step = step;

    fprintf(stderr, "[Master] constructor (OTA: %d)\n", (int)impl.ota);

    return &impl.base;
}

/* Server entry point. */
int main(void)
{
    NkKosTransport transport;
    ServiceId iid;

    /* Get server IPC handle of "master_connection". */
    Handle handle = ServiceLocatorRegister("master_connection", NULL, 0, &iid);
    assert(handle != INVALID_HANDLE);

    /* Initialize transport to client. */
    NkKosTransport_Init(&transport, handle, NK_NULL, 0);

    /**
     * Prepare the structures of the request to the server entity: constant
     * part and arena. Because none of the methods of the server entity has
     * sequence type arguments, only constant parts of the
     * request and response are used. Arenas are effectively unused. However,
     * valid arenas of the request and response must be passed to
     * server transport methods (nk_transport_recv, nk_transport_reply) and
     * to the server_entity_dispatch method.
     */
    Master_entity_req req;
    char req_buffer[Master_entity_req_arena_size];
    struct nk_arena req_arena = NK_ARENA_INITIALIZER(req_buffer,
                                        req_buffer + sizeof(req_buffer));

    /* Prepare response structures: constant part and arena. */
    Master_entity_res res;
    char res_buffer[Master_entity_res_arena_size];
    struct nk_arena res_arena = NK_ARENA_INITIALIZER(res_buffer,
                                        res_buffer + sizeof(res_buffer));

    /**
     * Initialize UCM Master component dispatcher. 3 is the value of the step,
     * which is the number by which the input value is increased.
     */
    /**
     * UCM Master object constructor.
     * step is the number by which the input value is increased.
     */
    ucm_Master_component component;
    ucm_Master_component_init(&component, CreateIMasterImpl(3));


    /* Initialize server entity dispatcher. */
    Master_entity entity;
    Master_entity_init(&entity, &component);

    fprintf(stderr, "[Hello] I'm UCM Master\n");



 
    /* Dispatch loop implementation. */
    do
    {
        /* Flush request/response buffers. */
        nk_req_reset(&req);
        nk_arena_reset(&req_arena);
        nk_arena_reset(&res_arena);

        /* Wait for request to server entity. */
        if (nk_transport_recv(&transport.base,
                              &req.base_,
                              &req_arena) != NK_EOK) {
            fprintf(stderr, "nk_transport_recv error\n");
        } else {
            /**
             * Handle received request by calling implementation CheckUpdates_Impl or Update_Impl
             * of the requested UCM Master interface method.
             */
            Master_entity_dispatch(&entity, &req.base_, &req_arena,
                                        &res.base_, &res_arena);
        }

        /* Send response. */
        if (nk_transport_reply(&transport.base,
                               &res.base_,
                               &res_arena) != NK_EOK) {
            fprintf(stderr, "nk_transport_reply error\n");
        }
    }
    while (true);

    return EXIT_SUCCESS;
}
