#ifndef common_h_
#define common_h_

#define _PICOG_STR(x) #x
#define PICOG_STR(x) _PICOG_STR(x)

#define _PICOG_CONCAT(x, y) x##y
#define PICOG_CONCAT(x, y) _PICOG_CONCAT(x, y)

#endif //common_h_