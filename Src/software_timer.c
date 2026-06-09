#include "software_timer.h"

#include <stddef.h>

static volatile uint32_t SoftwareTimerTickMs = 0;
static SoftwareTimer* TimerListHead = NULL;

static bool start_timer(SoftwareTimer* timer, uint32_t periodMs, bool periodic);
static bool timer_has_elapsed(const SoftwareTimer* timer, uint32_t nowMs);

void software_timer_init(SoftwareTimer* timer)
{
  if (timer == NULL)
  {
    return;
  }

  timer->next = NULL;
  timer->periodMs = 0;
  timer->deadlineMs = 0;
  timer->callback = NULL;
  timer->userData = NULL;
  timer->active = false;
  timer->periodic = false;
}

void software_timer_set_callback(SoftwareTimer* timer,
                                 SoftwareTimerCallback callback,
                                 void* userData)
{
  if (timer == NULL)
  {
    return;
  }

  timer->callback = callback;
  timer->userData = userData;
}

bool software_timer_register(SoftwareTimer* timer)
{
  if (timer == NULL)
  {
    return false;
  }

  timer->active = false;

  timer->next = TimerListHead;
  TimerListHead = timer;

  return true;
}

bool software_timer_start_oneshot(SoftwareTimer* timer, uint32_t periodMs)
{
  return start_timer(timer, periodMs, false);
}

bool software_timer_start_periodic(SoftwareTimer* timer, uint32_t periodMs)
{
  return start_timer(timer, periodMs, true);
}

void software_timer_stop(SoftwareTimer* timer)
{
  if (timer == NULL)
  {
    return;
  }

  timer->active = false;
}

bool software_timer_is_active(const SoftwareTimer* timer)
{
  if (timer == NULL)
  {
    return false;
  }

  return timer->active;
}

void software_timer_tick_1ms(void)
{
  SoftwareTimerTickMs++;
}

void software_timer_process(void)
{
  uint32_t nowMs = SoftwareTimerTickMs;
  SoftwareTimer* timer = TimerListHead;

  while (timer != NULL)
  {
    if (timer->active && timer_has_elapsed(timer, nowMs))
    {
      SoftwareTimerCallback callback = timer->callback;
      void* userData = timer->userData;

      if (timer->periodic)
      {
        timer->deadlineMs += timer->periodMs;
      }
      else
      {
        timer->active = false;
      }

      if (callback != NULL)
      {
        callback(userData);
      }
    }

    timer = timer->next;
  }
}

static bool start_timer(SoftwareTimer* timer, uint32_t periodMs, bool periodic)
{
  if (timer == NULL || timer->callback == NULL || periodMs == 0)
  {
    return false;
  }

  const uint32_t nowMs = SoftwareTimerTickMs;

  timer->periodMs = periodMs;
  timer->deadlineMs = nowMs + periodMs;
  timer->periodic = periodic;
  timer->active = true;

  return true;
}

static bool timer_has_elapsed(const SoftwareTimer* timer, uint32_t nowMs)
{
  return (int32_t)(nowMs - timer->deadlineMs) >= 0;
}
