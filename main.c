#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#ifdef _WIN32
#define CLEAR "cls"
#else
#define CLEAR "clear"
#endif

struct node {
  unsigned int tag;
  unsigned int block;
  int valid;
  unsigned int element;
};

int hit = 0;
int miss = 0;
int evictions = 0;

void printBinary(unsigned int num, unsigned int n) {
  int i, mask;
  for (i = n - 1; i >= 0; i--) {
    mask = 1 << i;
    printf("%d", (num & mask) ? 1 : 0);
  }
  printf("\t\t");
}

void display(struct node *cache, unsigned int lines, unsigned int tag_bits) {
  int i;
  system(CLEAR);
  printf(
      "┌════════════════════════════════════════════════════════════════┐\n");
  printf(
      "| Index |  Valid |      Tag      |       Block       |  Element  |\n");
  printf(
      "└════════════════════════════════════════════════════════════════┘\n");
  for (i = 0; i < lines; i++) {
    printf("|   %2u  |   %2d   | ", i, cache[i].valid);
    printBinary(cache[i].tag, tag_bits);
    printf(" |   %8u        |%8u   |\n", cache[i].block, cache[i].element);
  }
}

void directMapping(struct node *cache, unsigned int ref,
                   unsigned int offset_bits, unsigned int index_bits,
                   unsigned int tag_bits) {
  int offset = ref & ((1 << offset_bits) - 1);
  int temp = ref >> offset_bits;
  int index = temp & ((1 << index_bits) - 1);
  int tag = temp >> index_bits;

  cache[index].element = ref;

  if (cache[index].valid == 0) {
    printf("Miss\n");
    miss++;
    cache[index].tag = tag;
    cache[index].valid = 1;
    cache[index].block = temp;
  } else {
    if (cache[index].tag != tag) {
      printf("Miss\n");
      miss++;
      evictions++;
      cache[index].tag = tag;
      cache[index].block = temp;
    } else {
      printf("Hit\n");
      hit++;
    }
  }
}

int is_pwr_of_two(unsigned int n) { return (n & (n - 1)) == 0 && n != 0; }

int main() {
  int i;
  unsigned int mem_size, cache_size, block_size, offset_bits, lines, index_bits,
      tag_bits;
  unsigned int ref;

  printf("Main memory size: ");
  scanf("%u", &mem_size);
  while (mem_size <= 0 || !is_pwr_of_two(mem_size)) {
    printf("Is not greater than 1 or a power of 2. Try "
           "again!: \n\n");
    printf("Main memory size: ");
    scanf("%u", &mem_size);
  }

  printf("Cache size: ");
  scanf("%u", &cache_size);
  while (cache_size <= 0 || !is_pwr_of_two(cache_size)) {
    printf("Is not greater than 0 and a power of 2. Try"
           "again!: \n\n");
    printf("Cache Size: ");
    scanf("%u", &cache_size);
  }

  printf("Block size: ");
  scanf("%u", &block_size);
  while (block_size <= 0 || !is_pwr_of_two(block_size)) {
    printf("Is not greater than 0 and a power of 2. Try"
           "again: \n\n");
    printf("Enter the block size: ");
    scanf("%u", &block_size);
  }

  lines = cache_size / block_size;
  struct node cache[lines];
  offset_bits = log2(block_size);
  index_bits = log2(lines);
  tag_bits = log2(mem_size) - offset_bits - index_bits;

  for (i = 0; i < lines; i++) {
    cache[i].valid = 0;
    cache[i].block = 0;
    cache[i].tag = -1;
    cache[i].element = 0;
  }

  printf("┌═══════════════════════════════════════════════════════════════┐\n");
  display(cache, lines, tag_bits);
  printf(
      "└════════════════════════════════════════════════════════════════┘\n");

  unsigned int iterations = cache_size / block_size;

  for (unsigned int count = 0; count < iterations; count++) {
    printf("\nEnter main memory address as a decimal (Iteration %u/%u): ",
           count + 1, iterations);
    scanf("%u", &ref);
    directMapping(cache, ref, offset_bits, index_bits, tag_bits);
    printf("\n\n");
    printf(
        "└════════════════════════════════════════════════════════════════┘\n");

    display(cache, lines, tag_bits);
    printf(
        "└════════════════════════════════════════════════════════════════┘\n");

    printf(
        "┌════════════════════════════════════════════════════════════════┐\n");
    printf("Hits: %d Misses: %d Evictions: %d\n", hit, miss, evictions);
    printf(
        "└════════════════════════════════════════════════════════════════┘\n");
  }

  printf(
      "┌════════════════════════════════════════════════════════════════┐\n");
  display(cache, lines, tag_bits);
  printf(
      "└════════════════════════════════════════════════════════════════┘\n");
  printf(
      "┌════════════════════════════════════════════════════════════════┐\n");

  printf("Total hits: %d Total misses: %d Total evictions:%d\n", hit, miss,
         evictions);
  printf(
      "└════════════════════════════════════════════════════════════════┘\n");

  return 0;
}
