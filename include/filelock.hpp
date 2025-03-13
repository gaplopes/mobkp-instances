#ifndef FILELOCK_HPP
#define FILELOCK_HPP

// System includes for file locking functionality
#include <errno.h>     // For errno variable and error codes
#include <fcntl.h>     // File control options
#include <sys/file.h>  // File locking primitives (flock)
#include <unistd.h>    // POSIX API (close)

// Standard library includes
#include <iostream>   // For std::cerr
#include <stdexcept>  // For std::runtime_error
#include <string>     // For std::string

// External library includes
#include <fmt/format.h>  // For string formatting

/**
 * @brief RAII wrapper for file locking
 *
 * This class provides a Resource Acquisition Is Initialization (RAII) wrapper
 * around the flock system call, ensuring that file locks are properly acquired
 * and released, even in the presence of exceptions.
 */
class FileLock {
 public:
  /**
   * @brief Constructs a FileLock object and acquires an exclusive lock
   *
   * @param fd File descriptor to lock
   * @throws std::runtime_error if the lock cannot be acquired
   */
  explicit FileLock(int fd) : fd_(fd) {
    if (flock(fd_, LOCK_EX) == -1) {
      throw std::runtime_error(fmt::format("Failed to lock file: {}", strerror(errno)));
    }
  }

  /**
   * @brief Destroys the FileLock object and releases the lock
   *
   * Since this is a destructor, it is marked noexcept to prevent
   * potential termination if an exception is thrown during stack unwinding.
   * Errors are logged to std::cerr instead.
   */
  ~FileLock() noexcept {
    if (flock(fd_, LOCK_UN) == -1) {
      // Log error in destructor since we can't throw
      std::cerr << "Failed to unlock file: " << strerror(errno) << std::endl;
    }
  }

  // Prevent copying to avoid double unlocking
  /**
   * @brief Copy constructor (deleted)
   *
   * Copying a FileLock object would lead to multiple objects managing the same
   * lock, which could cause double-unlocking issues.
   */
  FileLock(const FileLock &) = delete;

  /**
   * @brief Copy assignment operator (deleted)
   *
   * Assignment would lead to resource management issues and is disallowed.
   */
  FileLock &operator=(const FileLock &) = delete;

 private:
  int fd_;  ///< File descriptor for the locked file
};

#endif  // FILELOCK_HPP
