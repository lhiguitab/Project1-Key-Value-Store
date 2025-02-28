# Key-Value Store Project

## Table of Contents
- [Introduction](#introduction)
- [Installation and Setup](#installation-and-setup)
- [Usage](#usage)
- [Implementation Details](#implementation-details)
- [Prerequisites](#prerequisites)
- [Dataset Requirements](#dataset-requirements)
- [Recommendations](#recommendations)
- [Authors](#authors)

## Introduction
This project involves the design and implementation of a Key-Value store. We will use the Key-Value store created to cache frequently accessed data within an application. The implementation of Key-Value storage will be done for game recommendations on Steam.

## Installation and Setup
Follow these steps to clone, compile, and run the project on your system:

1. Clone the repository:
   ```sh
   git clone https://github.com/lhiguitab/Project1-Key-Value-Store
   ```

2. Navigate to the project directory:
   ```sh
   cd Project1-Key-Value-Store
   ```

3. Create the executable:
   ```sh
   gcc -O3 main.c models.c csv_loader.c top_calculations.c -o dataload 
   ```

4. Run the executable:
   ```sh
   ./dataload -f FOLDER
   ```

## Usage
Once the executable is running, a menu will appear where you can:
* List the Top 10 most recommended games.
* List the Top 10 least recommended games.
* List the Top 10 users with the most recommendations made.
* View the Top 3 games most recommended by the top 10 users with the most recommendations.

## Implementation Details
The project was developed in **C** and utilizes the following data structures and algorithms:
- **Hash Tables:** Efficiently store and retrieve key-value pairs.
- **Arrays:** Manage data structures and perform quick lookup operations.
- **Quick Sort:** Optimize sorting operations for fast data retrieval.

## Prerequisites
Before running the project, ensure you have the following installed on your system:
- GCC Compiler (for compiling C programs)
- Git (to clone the repository)

## Dataset Requirements
To use this project effectively, you must download the following datasets:
- `games.csv`
- `recommendations.csv`
- `users.csv`

If you have not downloaded the datasets, you can find them at:
[Game Recommendations on Steam Dataset](https://www.kaggle.com/datasets/antonkozyriev/game-recommendations-on-steam)

## Recommendations
- Ensure the dataset files are placed in the correct directory before running the project.
- Verify that your system meets the prerequisites.

## Authors
- **Santiago Gómez Ospina**
- **Martin Valencia**
- **Lucas Higuita Bedoya**
