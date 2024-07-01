#include <fmt/color.h>
#include <string>

char mac[]="F4:13:0B:3D:3F:AD";

static unsigned char chartonum(char c)
{
  if (c >= '0' && c <= '9')
    return c - '0';
  if (c >= 'A' && c <= 'F')
    return (c - 'A') + 10;
  if (c >= 'a' && c <= 'f')
    return (c - 'a') + 10;
  return 0;
}

static char eth_mac[32] = {0};
unsigned char ETH_MAC[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
static int mac_addr_set(char *line)
{
  fmt::print("func :{} line:{}\n",__FUNCTION__,__LINE__);
  unsigned char mac[6];
  int i = 0;
  for (i = 0; i < 6 && line[0] != '\0' && line[1] != '\0'; i++)
  {
    mac[i] = chartonum(line[0]) << 4 | chartonum(line[1]);
    line += 3;
  }
  memcpy(ETH_MAC, mac, 6);
  snprintf(eth_mac, sizeof(eth_mac),"%02x:%02x:%02x:%02x:%02x:%02x",ETH_MAC[0], ETH_MAC[1], ETH_MAC[2],ETH_MAC[3], ETH_MAC[4], ETH_MAC[5]);
  fmt::print("eth {}\n",eth_mac);
  return 0;
}

int main()
{
  fmt::print(fg(fmt::color::crimson) | fmt::emphasis::bold,
             "Hello, {}!\n", "world");
  fmt::print("num {}\n",chartonum('9'));

  mac_addr_set(mac);
  return 0;
}