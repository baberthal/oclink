//
//  main.c
//  oclink
//
//  Created by Morgan Lieberthal on 2/23/16.
//  Copyright © 2016 J. Morgan Lieberthal. All rights reserved.
//

#include <stdio.h>
#include "clink.h"

int main(int argc, const char *argv[])
{
#pragma unused(argc, argv)
    OCL_Link *clink = ocl_link_alloc();
    if (!clink) {
        fprintf(stderr, "SOMETHING BAD HAPPENED.");
    }
    int rc = ocl_link_init(clink);
    if (rc != 0) {
        fprintf(stderr, "SOMETHING WENT WRONG INITIALIZING THE LINK\n");
        return -1;
    }

    int device_id = ocl_link_get_device_id(clink);
    printf("Device ID: %02x\n", device_id);

    int fw_version = ocl_link_get_fw_version(clink);
    printf("Firmware: %02x\n", fw_version);

    char name[255];
    ocl_link_get_product_name(clink, name);
    printf("Name: %s\n", name);

    return 0;
}
