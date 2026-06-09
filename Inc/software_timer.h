#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef void (*SoftwareTimerCallback)(void* userData);

typedef struct SoftwareTimer {
  struct SoftwareTimer* next;
  uint32_t periodMs;
  uint32_t deadlineMs;
  SoftwareTimerCallback callback;
  void* userData;
  bool active;
  bool periodic;
} SoftwareTimer;

void software_timer_init(SoftwareTimer* timer);
void software_timer_set_callback(SoftwareTimer* timer,
                                 SoftwareTimerCallback callback,
                                 void* userData);

bool software_timer_register(SoftwareTimer* timer);

bool software_timer_start_oneshot(SoftwareTimer* timer, uint32_t periodMs);
bool software_timer_start_periodic(SoftwareTimer* timer, uint32_t periodMs);
void software_timer_stop(SoftwareTimer* timer);

bool software_timer_is_active(const SoftwareTimer* timer);

void software_timer_tick_1ms(void);
void software_timer_process(void);
