cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

include(FindPackageHandleStandardArgs)

project(ChessGame LANGUAGES CXX VERSION 0.18.1)

set(ProjectName ChessGame)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)


set(ChessGame_HEADERS 
    src/Board.hpp
    src/DragEntity.hpp
    src/Entity.hpp
    src/EntityCommand.hpp
    src/Exit.hpp
    src/Game.hpp
    src/GameScene.hpp
    src/Menu.hpp
    src/Move.hpp
    src/Piece.hpp
    src/PieceBuilder.hpp
    src/Player.hpp
    src/Playing.hpp
    src/SceneManager.hpp
    src/Timestamp.hpp
)
set(ChessGame_SOURCES 
    src/main.cpp
    src/Board.cpp
    src/Exit.cpp
    src/Game.cpp
    src/Menu.cpp
    src/Piece.cpp
    src/Player.cpp
    src/Playing.cpp
    src/SceneManager.cpp
    src/Timestamp.cpp
)   
                      
add_executable(${ProjectName} ${ChessGame_HEADERS} ${ChessGame_SOURCES})

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/vendor/Stellar)
target_include_directories(${ProjectName} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/vendor/Stellar/Stellar)
target_link_libraries(${ProjectName} Stellar)