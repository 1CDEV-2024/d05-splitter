#include <assert.h>
#include <stdio.h>
#include <sys/stat.h>

static const char *SUFFIX[] = {"B", "KB", "MB", "GB", "TB"};
static const unsigned SFX_COUNT = sizeof(SUFFIX) / sizeof(*SUFFIX);

static char *human_readable_bytes(off_t bytes) {
  static char buffer[200] = {0};
  char length = sizeof(SUFFIX) / sizeof(*SUFFIX);
  int i = 0;
  double dbl_bytes = bytes;

  if (bytes > 1024) {
    for (i = 0; (bytes / 1024) > 0 && i < length - 1; i++, bytes /= 1024)
      dbl_bytes = bytes / 1024.0;
  }

  sprintf(buffer, "%.02lf %s", dbl_bytes, SUFFIX[i]);
  return buffer;
}

static void split_bytes(FILE *fp, unsigned bytes) {
  char buffer[bytes + 1];
  char *cursor;
  char c;

  FILE *split = NULL;
  char filename[30] = {0};
  char i = 0;

  do {
    cursor = buffer;
    while (cursor - buffer < bytes) {
      if ((c = fgetc(fp)) == EOF)
        break;
      *(cursor++) = c;
    }
    *cursor = '\0';
    if (c == EOF)
      break;
    sprintf(filename, "split%d", i++);
    assert((split = fopen(filename, "w")) != NULL);
    fprintf(split, "%s", buffer);
    fclose(split);
  } while (1);
}

int main(int ac, char **av) {
  assert(ac == 3);
  char *filename = av[1];
  char *byte_count = av[2];

  struct stat st;
  assert(stat(filename, &st) == 0);
  printf("File size: %s\n", human_readable_bytes(st.st_size));

  FILE *fp = fopen(filename, "r");
  assert(fp != NULL);

  unsigned part_size = 0;
  sscanf(byte_count, "%u", &part_size);
  split_bytes(fp, part_size);

  fclose(fp);
  return 0;
}
