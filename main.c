#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define LEN 200
#define LOG "contin.log"

// TODO : Add time/dates to the fprintf's.

struct uvec {
	const char *file_path;
	int counter;
	char **list;
};

/* A constructor for the uvec struct, this function allocates the space
 * needed for the new/updated 2d char array LIST then proceeds to 
 * populate it with information gathered from the selected file.
 *
 * This function requires two readthroughs of the selected file 
 * due to needing a count before space can be dynamically 
 * allocated for the LIST array.
 */

short
uvec_constructor (FILE *fp, struct uvec *vec) {
	int n;
	char **list;

	if ((fp = fopen(vec->file_path, "r")) == NULL) 
		return -1;
	
	vec->counter = 0;
	while (!feof(fp))
		if (fgetc(fp) == '\n') vec->counter++;
	fseek(fp, 0, SEEK_SET);

	vec->list = malloc((vec->counter + 1) * sizeof(char*));
	for (n = 0; n < vec->counter; n++) {
		vec->list[n] = malloc(LEN);
		fgets(vec->list[n], LEN, fp);
	}

	fclose(fp);
	return 0;
}

/* Opens up the file and compares each line against the most current
 * UVEC. Depending on modifcations found in each pass of this function
 * it will return a short integer.
 *
 * -1: This means this function failed at the starting line and could not
 *  	access the file.
 *
 *  0: This means there were no modifications found in this pass. Due to the
 *  	nature of opening a file with fopen, this does not mean that there weren't
 *  	any modifications during that pass. While this may be the case, any
 *  	modifications will be found in the following pass.
 *
 *  1: This means that there is a new entry into the file or an existing entry
 *  	was modified.
 *
 *  2: This means that a an entry in the file was deleted.
 */

short
check_vec (FILE *fp0, FILE *fp1, struct uvec *vec) {
	int i, n;
	short flag;
	char buf[LEN];
	
	if ((fp0 = fopen(LOG, "r+")) == NULL || (fp1 = fopen(vec->file_path, "r")) == NULL) {
		flag = -1;
		return flag;
	}

	fseek(fp0, 0, SEEK_END);

	flag = 0;
	for (i = 0; (fgets(buf, LEN, fp1) != NULL); i++) {
		if(i >= vec->counter) {
			flag = 1;
			fprintf(fp0, "[MOD] MODIFICATION IN %s:\n\t %s \t\tWAS ADDED.\n", 
					vec->file_path, buf);
			break;
		} if ((strcmp(vec->list[i], buf)) != 0) {
			flag = 1;
			for (n = 0; n < vec->counter; n++) {
				if (strcmp(vec->list[n], buf) == 0)
					flag = 0;
			} if (flag == 1) {
				fprintf(fp0, "[MOD] MODIFICATION IN %s:\n\t %s \t\tWAS ADDED.\n", 
						vec->file_path, buf);
			} 
			n = 0;
		}
		memset(&buf, 0, LEN);
	}

	if (vec->counter > i) {
		fprintf(fp0, "[MOD] MODIFICATION IN %s: ENTRY WAS DELETED\n",
				vec->file_path);
		flag = 2;
	}
	fclose(fp0);
	fclose(fp1);
	return flag;
}

int 
main () {
	int i, n, counter;
	FILE *fp0, *fp1;
	struct uvec *arr;
	char **file_paths;

	if ((fp0 = fopen("contin.conf", "r")) == NULL) 
		return -1;
	
	counter = 0;
	while (!feof(fp0))
		if (fgetc(fp0) == '\n') counter++;

	arr = malloc(counter * sizeof(struct uvec));
	file_paths = malloc(counter * sizeof(char));
	fseek(fp0, 0, SEEK_SET);

	for (i = 0; ; i++) {
		file_paths[i] = malloc(LEN);
		if ((fscanf(fp0, "%s", file_paths[i])) == EOF)
			break;
		arr[i].file_path = file_paths[i];	
	}

	fclose(fp0);

	if ((fp0 = fopen(LOG, "w+")) == NULL)
		return -1;
	for (i = 0; i < counter; i++)
		if ((uvec_constructor(fp1, &arr[i])) < 0)
			fprintf(fp0, "[ERR] ERROR OPENING %s\n", arr[i].file_path);
	fclose(fp0);
	while (1) {
		for (n = 0; n < i; n++) {
			if ((check_vec(fp0, fp1, &arr[n])) > 0)
			       uvec_constructor(fp1, &arr[n]);
		}
		usleep(100);
	}
	
	fclose(fp0);

	return 0;
}
