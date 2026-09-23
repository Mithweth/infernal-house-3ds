// lang.h
#pragma once

bool lang_init(const char *language);
void lang_close(void);

const char *lang_get(const char *key);
