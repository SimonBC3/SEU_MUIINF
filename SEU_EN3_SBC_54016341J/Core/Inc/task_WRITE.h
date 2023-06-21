#ifndef INC_TASK_WRITE_H_
#define INC_TASK_WRITE_H_

extern uint32_t global_time_it;
extern char* sensorName;

void write();
char get_alarm_char();
char get_char_by_sensor(float alarmValue);

#endif
