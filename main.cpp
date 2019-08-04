#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <vector>
#include <cmath>
#include <ctime>
#include <deque>

#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>


struct winsize terminalSize;

std::vector<double> perlinNoise;

double map(int val, int originalMin, int originalMax, int newMin, int newMax){
  if(val > originalMax) return newMax;
  else if(val < originalMin) return newMin;

  double ratio = (double)(newMax - newMin) / (originalMax - originalMin);
  return newMin + ((val - originalMin) * ratio);
}

double map(double val, int originalMin, int originalMax, int newMin, int newMax){
  if(val > originalMax) return newMax;
  else if(val < originalMin) return newMin;

  double ratio = (double)(newMax - newMin) / (originalMax - originalMin);
  return newMin + ((val - originalMin) * ratio);
}

int random(int min, int max){
  return rand() % max + min;
}

double dist(int Aa, int Ab, int Ba, int Bb){
  return std::sqrt(std::pow(Aa - Ba, 2) + std::pow(Ab - Bb, 2));
}

void perlinInit(int waveLength, std::vector<int> targets){
  int targetIndex = 0;
  perlinNoise.clear();
  perlinNoise.push_back(25);
  for(int i = 0; i < targets.size(); i++){
    double target = targets[targetIndex++];
    double wavelen = waveLength + random(-20, 20);
    while(std::abs(perlinNoise.back() - target) > 5){
      if(target > perlinNoise.back()){
        perlinNoise.push_back(perlinNoise.back() + random(1, 2));
      } else {
        perlinNoise.push_back(perlinNoise.back() - random(1, 2));
      }
    }
    perlinNoise.push_back(perlinNoise.back());
  }
}

void perlinInit(int waveLength, int vectorSize){
  perlinNoise.clear();
  perlinNoise.push_back(25);
  for(int i = 0; i < vectorSize; i++){
    double target = random(70, 100);
    double wavelen = waveLength + random(-20, 20);
    while(std::abs(perlinNoise.back() - target) > 5){
      if(target > perlinNoise.back()){
        perlinNoise.push_back(perlinNoise.back() + random(1, 2));
      } else {
        perlinNoise.push_back(perlinNoise.back() - random(1, 2));
      }
    }
    perlinNoise.push_back(perlinNoise.back());
  }
}

double getAudio(){
  std::vector<int16_t> samples(terminalSize.ws_col*2);
  char *path = "/tmp/mpd.fifo";
  int fd = open(path , O_RDONLY | O_NONBLOCK);
  if(fd < 0){
		fprintf(stderr, "Cannot open uhid-cdev %s: %m\n", path);
  }
  //int16_t *a = (int16_t*)malloc(sizeof(int16_t) * 1);
  ssize_t data = read(fd, samples.data(), samples.size() * sizeof(std::vector<int16_t>::value_type));
  //ssize_t data = read(fd, a, sizeof(a));
  if(data < 0)return 0.0;
  int waveLength= 0;
  int count = 0;
  int waves = 1;

  int test = 0;
  for(int i = 0; i < samples.size(); i+=2){
    waveLength++;
    if(i > 0 && i < samples.size() && samples[i - 1] < samples[i] && samples[i + 1] > samples[i]){
      //count += map(line[i], -127, 127, 0, 50);
      //count += map(waveLength, 0, 30, 0, 25);
      waveLength = 0;
      waves++;
    }
    test = std::max<double>(test, samples[i]);
  }
  std::string buffer;
  for(int i = 0; i < terminalSize.ws_row; i++){
    for(int j = 0; j < terminalSize.ws_col; j++){
      if((int)map(samples[j * 2], -32768, 32768, 0, terminalSize.ws_row) == i){
        buffer += "#";
        continue;
      }
      buffer += " ";
    }
  }
  //std::cout << buffer;
  std::flush(std::cout);

  close(fd);
  return test;
}


int main(){
  srand(time(NULL));
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &terminalSize);
  std::vector<std::pair<int, int>> screen;
  double density = 0.01;

  double b = 0.01;
  double increment = 0.1;

  double randomness = 0.5;

  std::cout << "\033[s";
  while(true){
    double val = getAudio();
    /*
    for(int i = 0; i < terminalSize.ws_col; i++){
      std::cout << " ";
    }
    std::cout << "\033[u";
    for(int i = 0; i < (int)map(val, -32768, 32768, terminalSize.ws_col, 0); i++){
      std::cout << "#";
    }
    std::cout << "\033[u";
    //if(val != 0)std::cout << val << std::endl;
    usleep(100000);
    continue;
    std::cout << "\033[u";
    for(int i = 0; i < terminalSize.ws_col; i++){
      std::cout << " ";
    }
    std::cout << "\033[u";
    for(int i = 0; i < (int)map((int)getAudio(), 0, 10, 0, terminalSize.ws_col); i++){
      std::cout << "*";
    }
    std::cout << "\033[u";
    std::flush(std::cout);
    */
    perlinInit(10, 300);

    randomness = map(val, 0, 32768, 0, 3);

    screen.clear();

    int r = 10 - randomness;

    for(double i = 0; i < M_PI * 2; i+=density){
      bool isOkay = true;
      int x, y;
      int randomVal = map(perlinNoise[map(std::cos(i) + std::sin(i), -2, 2, 0, 300)], 0, 100, 1 * randomness, 5 * randomness);
      x = (r + randomVal) * std::sin(i) / 2;
      y = (r + randomVal) * std::cos(i);

      for(auto point : screen){
        if(std::round(std::atan2(point.second, point.first) * 180 / M_PI) - std::round(std::atan2(y, x) * 180 / M_PI) >= 0 && std::round(std::atan2(point.second, point.first) * 180 / M_PI) - std::round(std::atan2(y, x) * 180 / M_PI) < 4){
          isOkay = false;
        }
      }
      if(isOkay)screen.push_back(std::pair<int, int>(x, y));
    }

    std::string buffer;
    for(int i = terminalSize.ws_row * -0.5; i < terminalSize.ws_row * 0.5; i++){
      for(int j = terminalSize.ws_col * -0.5; j < terminalSize.ws_col * 0.5; j++){
        std::pair<int, int> current;
        for(auto point : screen){
          if(point.first == i && point.second == j){
            current = point;
          }
        }
        if(current.first&&current.second){
          buffer += "■";
          continue;
        }
        buffer += " ";
      }
    }
    std::cout << buffer;
    std::flush(std::cout);
    buffer.clear();
    usleep(random(40000, 100000));
  }
}
