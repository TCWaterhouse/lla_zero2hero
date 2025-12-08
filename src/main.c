#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "file.h"
#include "parse.h"

void print_usage(char *argv[]) {
    printf("Usage: %s -f <database file> -n -a <employee name,address,hours> -l\n", argv[0]);
    printf("\t -f - (required) path to database file\n");
    printf("\t -n - create new database file\n");
    printf("\t -a - add employee to database via csv (name,address,hours)\n");
    printf("\t -l - list employees in database\n");
    return;
}

int main(int argc, char *argv[]) {
    char *filepath = NULL;
    unsigned short port = 0;
    bool newfile = false;
    bool list = false;
    char *addstring = NULL;
    int c;

    int dbfd = -1;
    struct dbheader_t *dbhdr = NULL;
    struct employee_t *employees = NULL;

    while ((c = getopt(argc, argv, "nf:a:l")) != -1) {
        switch (c) {
        case 'f':
            filepath = optarg;
            break;
        case 'n':
            newfile = true;
            break;
        case 'a':
            addstring = optarg;
            break;
        case 'l':
            list = true;
            break;
        case '?':
            printf("Unknown option -%c\n", c);
            break;
        default:
            return -1;
        }
    }

    if (filepath == NULL) {
        printf("Filepath is a required argument\n");
        print_usage(argv);
        return 0;
    }

    if (newfile) {
        dbfd = create_db_file(filepath);
        if (dbfd == STATUS_ERROR) {
            printf("Unable to create database file\n");
            return -1;
        }

        if (create_db_header(&dbhdr) == STATUS_ERROR) {
            printf("Failed to create database header\n");
            return -1;
        }
    } else {
        dbfd = open_db_file(filepath);
        if (dbfd == STATUS_ERROR) {
            printf("Unable to open database file\n");
            return -1;
        }

        if (validate_db_header(dbfd, &dbhdr) == STATUS_ERROR) {
            printf("Failed to validate database header\n");
            return -1;
        }
    }

    if (read_employees(dbfd, dbhdr, &employees) != STATUS_SUCCESS) {
        printf("Failed to read employees");
        return 0;
    }

    if (addstring) {
        add_employee(dbhdr, &employees, addstring);
    }

    if (list) {
        list_employees(dbhdr, employees);
    }

    output_file(dbfd, dbhdr, employees);

    return 0;
}
