
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/* Files required for transport initialization. */
#include <coresrv/nk/transport-kos.h>
#include <coresrv/sl/sl_api.h>

/* EDL description of the server entity. */
#include <ucm/OTA.edl.h>
#include <ucm/OTA.idl.h>

#include <assert.h>

/* Type of interface implementing object. */
typedef struct IOTAImpl {
    struct ucm_OTA base;     /* Base interface of object */
    rtl_uint32_t step;         /* Extra parameters */
} IOTAImpl;

static nk_err_t CheckUpdates_impl(struct ucm_OTA *self,
                          const struct ucm_OTA_CheckUpdates_req *req,
                          const struct nk_arena *req_arena,
                          struct ucm_OTA_CheckUpdates_res *res,
                          struct nk_arena *res_arena)
{
    // TODO: check internet connection
    IOTAImpl *impl = (IOTAImpl *)self;
    res->result = ucm_OTA_UpdatesAvailable;
    return NK_EOK;
}

static nk_err_t TransferPackage_impl(struct ucm_OTA *self,
                          const struct ucm_OTA_TransferPackage_req *req,
                          const struct nk_arena *req_arena,
                          struct ucm_OTA_TransferPackage_res *res,
                          struct nk_arena *res_arena)
{
    // TODO: download package from internet
    IOTAImpl *impl = (IOTAImpl *)self;
    res->result = ucm_OTA_TransferPackage;
    return NK_EOK;
}

/**
 * UCM OTA object constructor.
 * step is the number by which the input value is increased.
 */
static struct ucm_OTA *CreateIOTAImpl(rtl_uint32_t step)
{
    /* Table of implementations of UCM OTA interface methods. */
    static const struct ucm_OTA_ops ops = {
        .CheckUpdates = CheckUpdates_impl,
        .TransferPackage = TransferPackage_impl
    };

    /* Interface implementing object. */
    static struct IOTAImpl impl = {
        .base = {&ops}
    };

    impl.step = step;

    return &impl.base;
}

/* Server entry point. */
int main(void)
{
    NkKosTransport transport;
    ServiceId iid;

    /* Get server IPC handle of "OTA_connection". */
    Handle handle = ServiceLocatorRegister("ota_connection", NULL, 0, &iid);
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
    OTA_entity_req req;
    char req_buffer[OTA_entity_req_arena_size];
    struct nk_arena req_arena = NK_ARENA_INITIALIZER(req_buffer,
                                        req_buffer + sizeof(req_buffer));

    /* Prepare response structures: constant part and arena. */
    OTA_entity_res res;
    char res_buffer[OTA_entity_res_arena_size];
    struct nk_arena res_arena = NK_ARENA_INITIALIZER(res_buffer,
                                        res_buffer + sizeof(res_buffer));

    /**
     * Initialize UCM OTA component dispatcher. 3 is the value of the step,
     * which is the number by which the input value is increased.
     */
    ucm_OTA_component component;
    ucm_OTA_component_init(&component, CreateIOTAImpl(3));

    /* Initialize server entity dispatcher. */
    OTA_entity entity;
    OTA_entity_init(&entity, &component);

    fprintf(stderr, "[Hello] I'm OTA client\n");

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
             * Handle received request by calling implementation CheckUpdates_Impl or TransferPackage_Impl
             * of the requested UCM OTA interface method.
             */
            OTA_entity_dispatch(&entity, &req.base_, &req_arena,
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
