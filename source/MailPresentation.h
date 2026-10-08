#pragma once
#include <cstddef>
namespace MailPresentation {
inline const char* typeKey(size_t deliveries){return deliveries>1?"ui.multiple_message_delivery":"ui.message_delivery";}
}
