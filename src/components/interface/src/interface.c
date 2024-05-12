
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

/* Files required for transport initialization. */
#include <coresrv/nk/transport-kos.h>
#include <coresrv/sl/sl_api.h>

/* Description of the server interface used by the `client` entity. */
#include <ucm/Master.idl.h>

#include <assert.h>

#define EXAMPLE_VALUE_TO_SEND 777

/* Client entity entry point. */
int main(int argc, const char *argv[])
{
    NkKosTransport transport;
    struct ucm_Master_proxy proxy;

    fprintf(stderr, "[Hello] I'm Interface\n");

    /**
     * Get the client IPC handle of the connection named
     * "server_connection".
     */
    Handle handle = ServiceLocatorConnect("master_connection");
    assert(handle != INVALID_HANDLE);

    /* Initialize IPC transport for interaction with the server entity. */
    NkKosTransport_Init(&transport, handle, NK_NULL, 0);

    /**
     * Get Runtime Interface ID (RIID) for interface ucm.Master.master.
     * Here master is the name of the ucm.Master component instance,
     * ucm.Master.master is the name of the ucm.Master interface implementation.
     */
    nk_iid_t riid = ServiceLocatorGetRiid(handle, "ucm.Master.master");
    assert(riid != INVALID_RIID);

    /**
     * Initialize proxy object by specifying transport (&transport)
     * and server interface identifier (riid). Each method of the
     * proxy object will be implemented by sending a request to the server.
     */
    ucm_Master_proxy_init(&proxy, &transport.base, riid);


    do
    {
        sleep(1);
        {
            ucm_Master_CheckUpdates_req req;
            ucm_Master_CheckUpdates_res res;

            if (ucm_Master_CheckUpdates(&proxy.base, &req, NULL, &res, NULL) != rcOk)
            {
                fprintf(stderr, "[Interface] Failed to call ucm.Master.CheckUpdates\n");
                continue;
            }
            if (res.result == 2)
            {
                fprintf(stderr, "[Interface] Update isn't available\n");
                continue;
            }
        }

        {
            ucm_Master_Update_req req;
            ucm_Master_Update_res res;
            fprintf(stderr, "[Interface] Update available\n");
            if (ucm_Master_Update(&proxy.base, &req, NULL, &res, NULL) != rcOk)
            {
                fprintf(stderr, "[Interface] Update failed\n");
                continue;
            }
            fprintf(stderr, "[Interface] Update succeeded\n");
            return EXIT_SUCCESS; 
        }
    }
    while (1);

    return EXIT_SUCCESS;
}
