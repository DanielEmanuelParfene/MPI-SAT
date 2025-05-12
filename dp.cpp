#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

struct MyHash {
  std::size_t operator()(std::vector<short> const& vec) const {
    std::size_t seed = vec.size();
    for (auto& i : vec) {
      seed ^= i + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
    return seed;
  }
};

#define file std::ifstream
#define arr std::vector<short>
#define matrix std::vector<std::vector<short>>
#define u_map std::unordered_map<short, std::vector<int>>
#define map_ std::map<short, std::list<int>>
#define lst std::list<short>
#define u_map_1 std::unordered_map<std::string, bool>
#define u_map_2 std::unordered_map<std::vector<short>, bool, MyHash>

void read(file& f, matrix& clause_matrix, u_map& literals_to_clauses,
          map_& size_to_clauses, lst& unar_clauses, u_map_2& clauses_generated,
          int& clauses, int& literals) {
  int x, ind = 1;

  f >> literals >> clauses;

  arr vec(literals + 2, 0);
  clause_matrix.push_back(vec);

  while (f >> x) {
    if (x != 0) {
      if (x > 0) {
        vec[x] = 1;
      } else {
        vec[-x] = -1;
      }

      ++vec[0];
      literals_to_clauses[x].push_back(ind);
    } else {
      if (vec[0] == 1) {
        for (int i = 1; i <= literals; ++i) {
          if (vec[i] != 0) {
            unar_clauses.push_back(i * vec[i]);
            break;
          }
        }
      }

      size_to_clauses[vec[0]].push_back(ind);
      clause_matrix.push_back(vec);
      clauses_generated[vec] = true;
      for (int i = 0; i <= literals; ++i) {
        vec[i] = 0;
      }
      ++ind;
    }
  }
}

void create_new_clause(int index1, int index2, matrix& clause_matrix,
                       u_map& literals_to_clauses, map_& size_to_clauses,
                       lst& unar_clauses, u_map_2& clauses_generated,
                       int& clauses, int literals, int& k) {
  arr vec(literals + 2, 0);
  int cnt = 0, ind = clause_matrix.size();

  for (int i = 1; i <= literals; ++i) {
    if (clause_matrix[index1][i] != 0 || clause_matrix[index2][i] != 0) {
      if (clause_matrix[index1][i] * clause_matrix[index2][i] == -1) {
      } else if (clause_matrix[index1][i] != 0) {
        vec[i] = clause_matrix[index1][i];
        ++cnt;
      } else if (clause_matrix[index2][i] != 0) {
        vec[i] = clause_matrix[index2][i];
        ++cnt;
      }
    }
  }

  vec[0] = cnt;

  if (clauses_generated.find(vec) == clauses_generated.end()) {
    k = 1;

    for (int i = 1; i <= literals; ++i) {
      if (vec[i] == 1) {
        literals_to_clauses[i].push_back(ind);
      } else if (vec[i] == -1) {
        literals_to_clauses[-i].push_back(ind);
      }
    }

    ++clauses;
    size_to_clauses[cnt].push_back(ind);
    clause_matrix.push_back(vec);
    clauses_generated[vec] = true;

    if (cnt == 1) {
      for (int i = 1; i <= literals; ++i) {
        if (vec[i] != 0) {
          unar_clauses.push_back(i * vec[i]);
          break;
        }
      }
    } else if (cnt == 0) {
      std::cout << "S-a format clauza vida. Raspunsul este nesatisfiabil.";
      exit(0);
    }
  }
}

bool resolvent(int index1, int index2, matrix& clause_matrix, int literals) {
  int k = 0;
  for (int i = 1; i <= literals; ++i) {
    if (clause_matrix[index1][i] * clause_matrix[index2][i] == -1) {
      ++k;
      if (k > 1) {
        return false;
      }
    }
  }
  return k == 1;
}

bool combine(map_::iterator it1, map_::iterator it2, matrix& clause_matrix,
             u_map& literals_to_clauses, map_& size_to_clauses,
             lst& unar_clauses, u_map_1& clauses_pair_tried,
             u_map_2& clauses_generated, int& clauses, int literals) {
  if (it1 == it2) {
    std::list<int>::iterator itl1 = it1->second.begin();

    for (int i = 0; i < it1->second.size() - 1; ++i, ++itl1) {
      std::list<int>::iterator itl2 = ++itl1--;

      for (int j = i + 1; j < it1->second.size(); ++j, ++itl2) {
        std::string s = std::to_string(*itl1) + "+" +
                        std::to_string(clause_matrix[*itl1][literals + 1]) +
                        "-" + std::to_string(*itl2) + "+" +
                        std::to_string(clause_matrix[*itl2][literals + 1]);

        if (clauses_pair_tried.find(s) == clauses_pair_tried.end()) {
          if (resolvent(*itl1, *itl2, clause_matrix, literals)) {
            int k = 0;
            create_new_clause(*itl1, *itl2, clause_matrix, literals_to_clauses,
                              size_to_clauses, unar_clauses, clauses_generated,
                              clauses, literals, k);
            clauses_pair_tried[s] = true;
            if (k == 0) {
              continue;
            }
            return true;
          }
        }
        clauses_pair_tried[s] = true;
      }
    }
  } else {
    std::list<int>::iterator itl1 = it1->second.begin();

    for (int i = 0; i < it1->second.size(); ++i, ++itl1) {
      std::list<int>::iterator itl2 = it2->second.begin();

      for (int j = 0; j < it2->second.size(); ++j, ++itl2) {
        std::string s = std::to_string(*itl1) + "+" +
                        std::to_string(clause_matrix[*itl1][literals + 1]) +
                        "-" + std::to_string(*itl2) + "+" +
                        std::to_string(clause_matrix[*itl2][literals + 1]);
        if (clauses_pair_tried.find(s) == clauses_pair_tried.end()) {
          if (resolvent(*itl1, *itl2, clause_matrix, literals)) {
            int k = 0;
            create_new_clause(*itl1, *itl2, clause_matrix, literals_to_clauses,
                              size_to_clauses, unar_clauses, clauses_generated,
                              clauses, literals, k = 1);
            clauses_pair_tried[s] = true;
            if (k == 0) {
              continue;
            }
            return true;
          }
        }
        clauses_pair_tried[s] = true;
      }
    }
  }
  return false;
}

void search_shortest(matrix& clause_matrix, u_map& literals_to_clauses,
                     map_& size_to_clauses, lst& unar_clauses,
                     u_map_1& clauses_pair_tried, u_map_2& clauses_generated,
                     int& clauses, int literals) {
  for (map_::iterator it = size_to_clauses.begin(); it != size_to_clauses.end();
       ++it) {
    map_::iterator it1 = it;
    while (it1 != size_to_clauses.end()) {
      if (combine(it, it1, clause_matrix, literals_to_clauses, size_to_clauses,
                  unar_clauses, clauses_pair_tried, clauses_generated, clauses,
                  literals)) {
        return;
      }
      ++it1;
    }
  }
  std::cout << "Raspunsul este satisfiabil deoarece nu se mai pot genera "
               "clauze noi!\n";

  exit(0);
}

void delete_pure_literal(int literal, matrix& clause_matrix,
                         u_map& literals_to_clauses, map_& size_to_clauses,
                         int& clauses) {
  for (const int& item : literals_to_clauses[literal]) {
    if (clause_matrix[item][0] != 0) {
      size_to_clauses[clause_matrix[item][0]].remove(item);
      if (size_to_clauses[clause_matrix[item][0]].size() == 0) {
        size_to_clauses.erase(clause_matrix[item][0]);
      }

      clause_matrix[item][0] = 0;
      --clauses;

      if (clauses == 0) {
        std::cout
            << "S-a format multimea vida de clauze. Raspunsul este satisfiabil";
        exit(0);
      }
    }
  }

  literals_to_clauses.erase(literal);
}

void pure_literal(matrix& clause_matrix, u_map& literals_to_clauses,
                  map_& size_to_clauses, int& clauses, int literals) { 
  for (int i = 1; i <= literals; ++i) {
    int k1 = 0, k2 = 0;
    if (literals_to_clauses.find(i) != literals_to_clauses.end()) {
      for (const int item : literals_to_clauses[i]) {
        if (clause_matrix[item][0] != 0) {
          k1 = i;
          break;
        }
      }
    }

    if (literals_to_clauses.find(-i) != literals_to_clauses.end()) {
      for (const int item : literals_to_clauses[-i]) {
        if (clause_matrix[item][0] != 0) {
          k2 = -i;
          break;
        }
      }
    }

    if (k1 != 0 && k2 == 0) {
      delete_pure_literal(k1, clause_matrix, literals_to_clauses,
                          size_to_clauses, clauses);
    } else if (k1 == 0 && k2 != 0) {
      delete_pure_literal(k2, clause_matrix, literals_to_clauses,
                          size_to_clauses, clauses);
    }
  }
}

void unit_propagation(int literal, matrix& clause_matrix,
                      u_map& literals_to_clauses, map_& size_to_clauses,
                      lst& unar_clauses, u_map_2& clauses_generated,
                      int& clauses, int literals) {
  for (const int item : literals_to_clauses[literal]) {
    if (clause_matrix[item][0] != 0) {
      size_to_clauses[clause_matrix[item][0]].remove(item);
      if (size_to_clauses[clause_matrix[item][0]].size() == 0) {
        size_to_clauses.erase(clause_matrix[item][0]);
      }

      clause_matrix[item][0] = 0;
      --clauses;

      if (clauses == 0) {
        std::cout
            << "S-a format multimea vida de clauze. Raspunsul este satisfiabil";
        exit(0);
      }
    }
  }

  if (literals_to_clauses.find(literal * -1) != literals_to_clauses.end()) {
    for (const int& item : literals_to_clauses[literal * -1]) {
      if (clause_matrix[item][0] != 0) {
        clause_matrix[item][abs(literal)] = 0;

        size_to_clauses[clause_matrix[item][0]].remove(item);
        if (size_to_clauses[clause_matrix[item][0]].size() == 0) {
          size_to_clauses.erase(clause_matrix[item][0]);
        }

        --clause_matrix[item][0];
        int aux = ++clause_matrix[item][literals + 1];
        size_to_clauses[clause_matrix[item][0]].push_back(item);
        clause_matrix[item][literals + 1] = 0;
        clauses_generated[clause_matrix[item]] = true;
        clause_matrix[item][literals + 1] = aux;

        if (clause_matrix[item][0] == 0) {
          std::cout << "S-a format clauza vida. Raspunsul este nesatisfiabil.";
          exit(0);
        } else if (clause_matrix[item][0] == 1) {
          for (int i = 1; i <= literals; ++i) {
            if (clause_matrix[item][i] != 0) {
              unar_clauses.push_back(i * clause_matrix[item][i]);
              break;
            }
          }
        }
      }
    }
  }

  literals_to_clauses.erase(literal);
  literals_to_clauses.erase(literal * -1);

  unar_clauses.remove(literal);
}

void DP(matrix& clause_matrix, u_map& literals_to_clauses,
        map_& size_to_clauses, lst& unar_clauses, u_map_1& clauses_pair_tried,
        u_map_2& clauses_generated, int& clauses, int literals) {
  int k = 0;
  while (true) {
    if (unar_clauses.size() != 0) {
      lst::iterator it = unar_clauses.begin();
      int literal = *it;

      unit_propagation(literal, clause_matrix, literals_to_clauses,
                       size_to_clauses, unar_clauses, clauses_generated,
                       clauses, literals);

      k = 1;
    } else {
      if (k == 1) {
        pure_literal(clause_matrix, literals_to_clauses, size_to_clauses,
                     clauses, literals);

        k = 0;
      } else {
        search_shortest(clause_matrix, literals_to_clauses, size_to_clauses,
                        unar_clauses, clauses_pair_tried, clauses_generated,
                        clauses, literals);
      }
    }
  }
}

int main() {
  file f("input.txt");
  matrix clause_matrix;
  u_map literals_to_clauses;
  map_ size_to_clauses;
  lst unar_clauses;
  u_map_1 clauses_pair_tried;
  u_map_2 clauses_generated;
  int clauses, literals;

  read(f, clause_matrix, literals_to_clauses, size_to_clauses, unar_clauses,
       clauses_generated, clauses, literals);

  DP(clause_matrix, literals_to_clauses, size_to_clauses, unar_clauses,
     clauses_pair_tried, clauses_generated, clauses, literals);

  return 0;
}