// Forked from https://github.com/benblack769/sysHW2/blob/master/hct.cpp

#include <cstring>
#include <ctime>
#include <iostream>
#include <random>

namespace {

constexpr uint64_t kNumIters = (1 << 28);
constexpr int64_t kBufferSizePow2Low = 13;
constexpr int64_t kBufferSizePow2High = 27;

struct list {
  list* next;
};

uint64_t get_time_ns() {
  timespec t;
  clock_gettime(0, &t);
  return 1000000000ULL * t.tv_sec + t.tv_nsec;
}

list* create_random_linked_list(void* buff, const size_t size) {
  std::random_device seed_gen;
  std::mt19937_64 rand64(seed_gen());

  memset(buff, 0, size);

  auto* list_head = reinterpret_cast<list*>(buff);
  size_t list_len = size / sizeof(list*);

  list* cur = list_head;
  for (size_t i = 0; i < list_len; i++) {
    // find a next empty space
    size_t idx;
    do {
      idx = rand64() % list_len;
    } while (list_head[idx].next);

    cur->next = &list_head[idx];
    cur = cur->next;
  }
  // Link the tail to the head
  cur->next = list_head;

  return list_head;
}

double measure_random_acesses(void* buff,
                              const size_t size,
                              const uint64_t num_iters) {
  auto* list = create_random_linked_list(buff, size);
  auto t = get_time_ns();

  double res;
  for (uint64_t i = 0; i < num_iters; i++) {
    list = list->next;
  }
  res = (get_time_ns() - t) / static_cast<double>(num_iters);

  return res;
}

}  // namespace

int main() {
  std::cout << "\tBuffer [KiB]" << "\t\tLatency [ns]" << std::endl;

  for (int i = kBufferSizePow2Low; i < kBufferSizePow2High; i++) {
    const int64_t num_bt = 2;
    for (int64_t b = (1 << num_bt) - 1; b >= 0; b--) {
      uint64_t size = (1ULL << i) - (b << (i - num_bt - 1));
      void* buff = malloc(size);
      auto time = measure_random_acesses(buff, size, kNumIters);
      free(buff);

      printf("\t%.2f \t\t\t%.4f\n", size / 1024.0, time);
    }
  }
  return 0;
}
