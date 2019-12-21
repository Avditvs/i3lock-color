#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <linux/limits.h>
#include <regex.h>

#define _DATADIR "/sys/class/power_supply"

float get_battery_level() {
  FILE *f_c, *f_f;
  long current, full;
  DIR *d;
  struct dirent *dp;
  char b[PATH_MAX]; 
  float level;

  if((d = opendir(_DATADIR)) == NULL) {
    fprintf(stderr, "opendir: %s\n", strerror(errno));
    return 3;
  }

  while((dp = readdir(d)) != NULL) {
    snprintf(b, PATH_MAX, "%s/%s", _DATADIR, dp->d_name);

    regex_t regex;
    if(regcomp(&regex, "BAT[[:alnum:]]+", REG_EXTENDED) != 0) {
      fprintf(stderr, "regcomp: %s\n", strerror(errno));
      return 4;
    }
    if(regexec(&regex, b, 0, NULL, 0) == 0) {
      snprintf(b, PATH_MAX, "%s/%s/%s", _DATADIR, dp->d_name, "charge_now");
      f_c = fopen(b, "r");
      snprintf(b, PATH_MAX, "%s/%s/%s", _DATADIR, dp->d_name, "charge_full");
      f_f = fopen(b, "r");
      if(f_c != NULL && f_f != NULL) {
        if(fscanf(f_c, "%ld", &current) != 1 || fscanf(f_f, "%ld", &full) != 1)
          fprintf(stderr, "fscanf: %s\n", strerror(errno));
        else{
            level=((float)current / (float)full) * 100.0;
        }
        fclose(f_c);
        fclose(f_f);
      }
    }
    regfree(&regex);
  }
  return level;
}