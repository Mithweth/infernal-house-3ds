// lang.h
#pragma once

bool lang_init(void);
void lang_close(void);
void lang_next(void);
const char *lang_get(const char *key);
