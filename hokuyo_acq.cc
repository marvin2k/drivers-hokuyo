#include "hokuyo.hh"
#include <iostream>
#include <sys/time.h>
#include <time.h>
#include <math.h>

using namespace std;

static bool keypressed()
{
    int in = fileno(stdin);
    fd_set set;
    FD_ZERO(&set);
    FD_SET(in, &set);

    timeval timeout = { 0, 0 };
    if (select(in + 1, &set, NULL, NULL, &timeout) == 0)
        return false;
    return true;
}

int main (int argc, const char** argv){
  if (argc<2){
    printf( "Usage: %s [-uri] <device> ", argv[0]);
    return 0;
  }

  URG urg;
  if (string(argv[1]) == "-uri")
  {
      urg.openURI(argv[2]);
  }
  else
  {
      if (!urg.setBaudrate(115200))
      {
          cerr << "cannot set baudrate: " << urg.errorString() << endl;
          perror("errno is");
          return 1;
      }

      if (!urg.open(argv[1]))
      {
          cerr << "cannot open device: " << urg.errorString() << endl;
          perror("errno is");
          return 1;
      }
  }

  cerr << urg.getInfo() << endl;

  if (!urg.startAcquisition(0))
  {
      cerr << "cannot start acquisition: " << urg.errorString() << endl;
      perror("errno is");
      return 1;
  }

  base::samples::LaserScan scan;
  cout << "# timestamp is in milliseconds, angles are in radians and ranges in millimeters." << endl;
  cout << "# timestamp angle_min angle_resolution range_count range0 range1 ..." << endl;
  while(!keypressed())
  {
      if (!urg.readRanges(scan))
      {
          cerr << "failed acquisition" << endl;
          break;
      }


      float res   = scan.angular_resolution;
      float min   = scan.start_angle;

      cout << base::Time::now().toMilliseconds() << " " << scan.time.toMilliseconds() << " " << min << " " << res << " " << scan.ranges.size();
      for (size_t i = 0; i < scan.ranges.size(); ++i)
          cout << " " << scan.ranges[i];
      cout << endl;
  }

  urg.stopAcquisition();
  urg.close();
}

