//
//  main.c
//  oclink
//
//  Created by Morgan Lieberthal on 2/23/16.
//  Copyright © 2016 J. Morgan Lieberthal. All rights reserved.
//

#include <clink.h>
#include <stdio.h>

int main(int argc, const char *argv[])
{
    OCL_Link *link = ocl_link_shared_link();
    if (!link) {
        fprintf(stderr, "SOMETHING BAD HAPPENED.");
    }

    int device_id = ocl_link_get_device_id(link);
    printf("Device ID: %02x\n", device_id);

    int fw_version = ocl_link_get_fw_version(link);
    printf("Firmware: %02x\n", fw_version);

    char name[255];
    ocl_link_get_product_name(link, name);
    printf("Name: %s\n", name);

    return 0;
}
