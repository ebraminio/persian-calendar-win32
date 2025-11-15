#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "persian-calendar.h"

#define MAX_LINE_LENGTH 256

int main()
{
    FILE *file = fopen("test.txt", "r");
    if (!file)
    {
        perror("Error opening file");
        return EXIT_FAILURE;
    }

    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file))
    {
        uint32_t gy, gm, gd, py, pm, pd;
        if (sscanf(line, "%d,%d,%d,%d,%d,%d", &gy, &gm, &gd, &py, &pm, &pd) == 6)
        {
            uint32_t jdn = gregorian_to_jdn(gy, gm, gd);
            persian_date_t persian_date = jdn_to_persian(jdn);
            if (persian_date.year != py || persian_date.month != pm || persian_date.day != pd)
            {
                printf("Error in %s", line);
            }
        }
        else
        {
            fprintf(stderr, "Invalid line format: %s", line);
        }
    }

    fclose(file);
    return EXIT_SUCCESS;
}