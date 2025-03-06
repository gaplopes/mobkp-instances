#ifndef FILELOCK_HPP
#define FILELOCK_HPP

#include <fcntl.h>
#include <stdexcept>
#include <string>
#include <fmt/format.h>
#include <iostream>
#include <unistd.h>
#include <sys/file.h>
#include <errno.h>

class FileLock {
  public:
   explicit FileLock(int fd) : fd_(fd) {
     if (flock(fd_, LOCK_EX) == -1) {
       throw std::runtime_error(fmt::format("Failed to lock file: {}", strerror(errno)));
     }
   }
 
   ~FileLock() noexcept {
     if (flock(fd_, LOCK_UN) == -1) {
       // Log error in destructor since we can't throw
       std::cerr << "Failed to unlock file: " << strerror(errno) << std::endl;
     }
   }
 
   // Prevent copying
   FileLock(const FileLock &) = delete;
   FileLock &operator=(const FileLock &) = delete;
 
  private:
   int fd_;
 };

#endif  // FILELOCK_HPP
