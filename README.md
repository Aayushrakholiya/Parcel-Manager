<div align="center">

# 📦 Parcel Manager

**A C data-structures engine that indexes thousands of courier parcels in a hash-table-of-BSTs: O(1) bucket lookup combined with ordered tree traversal for fast search, totals, and min/max analytics.**

C · Hash Tables · Binary Search Trees · DJB2 Hashing

<br>

![C](https://img.shields.io/badge/C-A8B9CC?style=flat-square&logo=c&logoColor=black)
![Hash Table](https://img.shields.io/badge/Hash_Table_127_buckets-blue?style=flat-square)
![BST](https://img.shields.io/badge/Binary_Search_Tree-green?style=flat-square)
![Visual Studio](https://img.shields.io/badge/Visual_Studio-5C2D91?style=flat-square&logo=visualstudio&logoColor=white)

</div>

---

## 📌 Table of Contents

- [About](#-about)
- [The Data Structure](#-the-data-structure)
- [Features](#-features)
- [Getting Started](#-getting-started)

## 👋 About

A console app built for SENG1050 that loads raw courier data (destination, weight, valuation — up to 5,000 parcels) and organizes it using a **hybrid hash table + binary search tree** design. Each of the 127 hash buckets holds the root of a BST keyed by destination, so lookups get the best of both worlds: hash-speed bucket routing and ordered-tree search within it.

## 🧠 The Data Structure

```text
              ParcelHashTable (127 buckets)
  ┌─────────┬─────────┬─────────┬─────┬─────────┐
  │ bucket0 │ bucket1 │ bucket2 │ ... │ bucket126│
  └────┬────┴────┬────┴─────────┴─────┴──────────┘
       │         │
       ▼         ▼
     BST       BST        each node = ParcelNode
   (dest key) (dest key)   { destination · weight · valuation }
```

- **Hashing** — DJB2 algorithm (initial value 5381) maps each destination to one of 127 buckets
- **BST per bucket** — collisions are handled by chaining into a binary search tree sorted by destination name
- **Traversal** — in-order walks power the print, total, and min/max reports

## ✨ Features

The interactive menu:

| # | Option | What it does |
| --- | --- | --- |
| 1 | Print all parcels | In-order traversal of every bucket's tree |
| 2 | Print parcel pair | Find a parcel by destination (tree search) |
| 3 | Print totals | Accumulated weight + valuation across all parcels |
| 4 | Min/Max flight price | Cheapest and most expensive parcel |
| 5 | Min/Max weight | Lightest and heaviest parcel |
| 6 | Exit | Frees the entire table and every node cleanly |

Data sanity is enforced at load: weights are kept between **100 g and 50,000 g**.

## 🚀 Getting Started

**Prerequisites:** any C compiler (built with Visual Studio — `ParcelManager.sln` included).

```bash
# compile directly
gcc ParcelManager.cpp -o parcel-manager
./parcel-manager

# or open ParcelManager.sln in Visual Studio and press F5
```

Keep `couriers.txt` (CSV: `destination, weight, valuation`) in the working directory — it's loaded on startup.

---

<div align="center">

⭐ **Star this repo if you find it useful** ⭐

</div>
