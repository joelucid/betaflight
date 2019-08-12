
// Generic interval timer service

typedef struct iTimerUs_s {
    timeUs_t duration;
    timeUs_t remaining;
    void (*callback)(void);
    strcut iTimerUs_s next;
} iTimerUs_t;

void iTimerMicros(iTimerUs_t *iTimerUs);

provides 1us granularity callback service

Run 1MHz clock timer (TIM6 or TIM7)

Manage requests in a list
