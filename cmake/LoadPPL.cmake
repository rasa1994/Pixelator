include(FetchContent)
FetchContent_Declare(
  processppl
  GIT_REPOSITORY https://github.com/rasa1994/ProcessParallelization.git
  GIT_TAG master
)

FetchContent_MakeAvailable(processppl)