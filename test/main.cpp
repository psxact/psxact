#include <rapidcheck.h>

int main() {
  rc::check("testing", [](int a) {
    RC_ASSERT(a == a);
  });

  return 0;
}
