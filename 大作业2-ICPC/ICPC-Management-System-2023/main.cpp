// #pragma GCC optimze("Ofast", 3, "inline")
#include <algorithm>
#include <functional>
#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <string>
#include <unordered_map>
#include <stack>
// #include "debug.h"

// 题目状态
struct ProblemStatus {
    bool is_solved;
    int attempt_count;
    int time;
    ProblemStatus() : is_solved(false), attempt_count(0), time(0) {}
    int penalty() const {
        return is_solved ? time + 20 * (attempt_count - 1) : 0;
    }
};

// 队伍信息
struct TeamInfo {
    std::string name;
    int id;
    int solved_count;
    int penalty;
    std::vector<ProblemStatus> prob_status, freeze_prob_status;
    std::vector<int> freeze_attempt_count;
    std::vector<int> ord_time;
};

// 定义两队排名比较大小的方法
bool operator<(const TeamInfo &lhs, const TeamInfo &rhs)  {
    if (lhs.solved_count != rhs.solved_count) {
        return lhs.solved_count > rhs.solved_count;
    }
    if (lhs.penalty != rhs.penalty) {
        return lhs.penalty < rhs.penalty;
    }
    for (int i = lhs.solved_count - 1; i >= 0; --i) {
        if (lhs.ord_time[i] != rhs.ord_time[i]) {
            return lhs.ord_time[i] < rhs.ord_time[i];
        }
    }
    return lhs.name < rhs.name;
}

// 判题状态
enum kJudgeStatus {
    ALL,
    Accepted,
    Wrong_Answer,
    Runtime_Error,
    Time_Limit_Exceed
};

// 提交状态
struct SubmitStatus {
    int team_id;
    int problem_id;
    int time;
    kJudgeStatus status;
};

// 记录最后一次提交 last_submit[team_id][problem_id][status] = status
std::vector<std::vector<std::vector<SubmitStatus>>> last_submit;

struct BoardCmp {
    bool operator()(const int &team1_id, const int &team2_id) const;
};

std::unordered_map<std::string, int> team_name_to_id;
int team_count, problem_count, duration_time;
std::vector<int> rank_of_team;
std::vector<TeamInfo> team_info;
std::set<int, BoardCmp> board;
bool is_contest_started = false;
bool is_freeze = false;

bool BoardCmp::operator()(const int &team1_id, const int &team2_id) const {
    return team_info[team1_id] < team_info[team2_id];
}

// 添加队伍 O(logn)
void AddTeam(std::string team_name) {
    if (is_contest_started) {
        std::cout << "[Error]Add failed: competition has started.\n";
        return;
    }
    if (team_name_to_id.count(team_name)) {
        std::cout << "[Error]Add failed: duplicated team name.\n";
        return;
    }
    team_name_to_id[team_name] = team_count;
    team_info.push_back({team_name, team_count, 0, 0, std::vector<ProblemStatus>(), std::vector<ProblemStatus>()});
    rank_of_team.push_back(0);
    board.insert(team_count);
    ++team_count;
    std::cout << "[Info]Add successfully.\n";
}

// 开始比赛 O(n)
void Start(int duration_time, int problem_count) {
    if (is_contest_started) {
        std::cout << "[Error]Start failed: competition has started.\n";
        return;
    }
    ::duration_time = duration_time;
    ::problem_count = problem_count;
    is_contest_started = true;
    last_submit.resize(team_count);
    for (int i = 0; i < team_count; ++i) {
        last_submit[i].resize(problem_count + 1);
        for (int j = 0; j <= problem_count; ++j) {
            last_submit[i][j].resize(5, {-1, -1, -1, ALL});
        }
        team_info[i].prob_status.resize(problem_count);
        team_info[i].freeze_prob_status.resize(problem_count);
        team_info[i].freeze_attempt_count.resize(problem_count);
    }
    int rank = 0;
    for (auto it = board.begin(); it != board.end(); ++it) {
        rank_of_team[*it] = ++rank;
    }
    std::cout << "[Info]Competition starts.\n";
}

// 刷新榜单，更新排名 O(n)
void Flush() {
    int rank = 0;
    for (auto it = board.begin(); it != board.end(); ++it) {
        rank_of_team[*it] = ++rank;
    }
    std::cout << "[Info]Flush scoreboard.\n";
}

// 查找排名 O(1)
void QueryRanking(std::string team_name) {
    if (team_name_to_id.count(team_name) == 0) {
        std::cout << "[Error]Query ranking failed: cannot find the team.\n";
        return;
    }
    int id = team_name_to_id[team_name];
    std::cout << "[Info]Complete query ranking.\n";
    if (is_freeze) std::cout <<
                                 "[Warning]Scoreboard is frozen. The ranking may be inaccurate until it were scrolled.\n";
    std::cout << team_name + " NOW AT RANKING " + std::to_string(
                  rank_of_team[id]) + "\n";
}

// 将枚举类型转换为字符串 O(1)
std::string to_string(kJudgeStatus sta) {
    switch (sta) {
    case ALL:
        return "ALL";
    case Accepted:
        return "Accepted";
    case Wrong_Answer:
        return "Wrong_Answer";
    case Runtime_Error:
        return "Runtime_Error";
    case Time_Limit_Exceed:
        return "Time_Limit_Exceed";
    default:
        return "Unknown";
    }
}

// 将字符串转换为枚举类型 O(1)
kJudgeStatus GetStatus(std::string status) {
    if (status == "Accepted") {
        return Accepted;
    } else if (status == "Wrong_Answer") {
        return Wrong_Answer;
    } else if (status == "Runtime_Error") {
        return Runtime_Error;
    } else if (status == "Time_Limit_Exceed") {
        return Time_Limit_Exceed;
    } else {
        return ALL;
    }
}

// 查询提交 O(1)
void QuerySubmission(std::string team_name, std::string prob_name,
                     kJudgeStatus status) {
    if (team_name_to_id.count(team_name) == 0) {
        std::cout << "[Error]Query submission failed: cannot find the team.\n";
        return;
    }
    std::cout << "[Info]Complete query submission.\n";
    int team_id = team_name_to_id[team_name];
    int prob_id = prob_name == "ALL" ? problem_count : prob_name[0] - 'A';
    if (last_submit[team_id][prob_id][status].time == -1) {
        std::cout << "Cannot find any submission.\n";
    } else {
        auto sta = last_submit[team_id][prob_id][status];
        std::cout << team_name << " " << char('A' + sta.problem_id) << " " << to_string(
                      sta.status) << " " << sta.time << "\n";
    }
}

// 提交更改 O(logn)
void Submit(std::string prob_name, std::string team_name, kJudgeStatus status,
            int time) {
    int prob_id = prob_name[0] - 'A';
    int team_id = team_name_to_id[team_name];
    last_submit[team_id][prob_id][status] = {team_id, prob_id, time, status};
    last_submit[team_id][problem_count][status] = {team_id, prob_id, time, status};
    last_submit[team_id][prob_id][ALL] = {team_id, prob_id, time, status};
    last_submit[team_id][problem_count][ALL] = {team_id, prob_id, time, status};
    if (!is_freeze) {
        // auto it = board.find(team_id);
        // auto kt = it == board.begin() ? next(it) : prev(it);
        auto it = board.erase(team_id);
        if (!team_info[team_id].prob_status[prob_id].is_solved) {
            team_info[team_id].prob_status[prob_id].attempt_count++;
            team_info[team_id].prob_status[prob_id].time = time;
            if (status == Accepted) {
                team_info[team_id].prob_status[prob_id].is_solved = true;
                team_info[team_id].solved_count++;
                team_info[team_id].penalty += team_info[team_id].prob_status[prob_id].penalty();
                team_info[team_id].ord_time.push_back(time);
            }
        }
        board.insert(team_id);
    } else {
        team_info[team_id].freeze_attempt_count[prob_id]++;
        if (!team_info[team_id].freeze_prob_status[prob_id].is_solved) {
            team_info[team_id].freeze_prob_status[prob_id].is_solved |= status == Accepted;
            team_info[team_id].freeze_prob_status[prob_id].attempt_count++;
            team_info[team_id].freeze_prob_status[prob_id].time = time;
        }
    }
}

// 冻结榜单 O(n)
void Freeze() {
    if (is_freeze) {
        std::cout << "[Error]Freeze failed: scoreboard has been frozen.\n";
        return;
    }
    is_freeze = true;
    for (int i = 0; i < team_count; ++i) {
        team_info[i].freeze_prob_status = team_info[i].prob_status;
        team_info[i].freeze_attempt_count = std::vector<int>(problem_count, 0);
    }
    std::cout << "[Info]Freeze scoreboard.\n";
}

// 格式化输出 O(1)
std::string FormatX(std::string op, int x) {
    return (x == 0) ? (op == "+" ? "+" : "0") : op + std::to_string(x);
}

// 遍历并输出榜单 O(n)
void PrintScoreBoard() {
    int rank = 0;
    for (auto it = board.begin(); it != board.end(); ++it) {
        rank_of_team[*it] = ++rank;
    }
    for (int team_id : board) {
        std::cout << team_info[team_id].name << " " << rank_of_team[team_id] << " " <<
                  team_info[team_id].solved_count << " " << team_info[team_id].penalty;
        for (int i = 0; i < problem_count; ++i) {
            if (team_info[team_id].prob_status[i].is_solved) {
                std::cout << " " << FormatX("+",
                                            team_info[team_id].prob_status[i].attempt_count - 1);
            } else if (team_info[team_id].prob_status[i].attempt_count ==
                       team_info[team_id].freeze_prob_status[i].attempt_count) {
                std::cout << " " << (team_info[team_id].prob_status[i].attempt_count == 0 ?
                                     "." :
                                     FormatX("-", team_info[team_id].prob_status[i].attempt_count));
            } else {
                std::cout << " " << FormatX("-",
                                            team_info[team_id].prob_status[i].attempt_count) << "/" <<
                          team_info[team_id].freeze_attempt_count[i];
            }
        }
        std::cout << "\n";
    }
}

// 滚榜 O(nlogn)
void Scroll() {
    if (!is_freeze) {
        std::cout << "[Error]Scroll failed: scoreboard has not been frozen.\n";
        return;
    }
    is_freeze = false;
    std::cout << "[Info]Scroll scoreboard.\n";
    PrintScoreBoard();
    auto it = --board.end();
    while (true) {
        bool fg = 1;
        int team_id = *it;
        for (int i = 0; i < problem_count; ++i) {
            if (!team_info[team_id].prob_status[i].is_solved
                && team_info[team_id].freeze_prob_status[i].is_solved) {
                int last = it == board.begin() ? -1 : *prev(it);
                auto tmp_info = team_info[team_id];
                tmp_info.prob_status[i] = tmp_info.freeze_prob_status[i];
                tmp_info.solved_count++;
                tmp_info.penalty += tmp_info.prob_status[i].penalty();
                tmp_info.ord_time.push_back(tmp_info.prob_status[i].time);
                int _i = tmp_info.ord_time.size() - 1;
                while (_i != 0 && tmp_info.ord_time[_i] < tmp_info.ord_time[_i - 1]) {
                    std::swap(tmp_info.ord_time[_i], tmp_info.ord_time[_i - 1]);
                    --_i;
                }
                
                if (last != -1 && tmp_info < team_info[last]) {
                    auto kt = it;
                    --it;
                    board.erase(kt);
                    team_info[team_id] = tmp_info;
                    auto jt = board.insert(it, team_id);
                    std::cout << team_info[team_id].name << " " << team_info[*++jt].name << " " <<
                              team_info[team_id].solved_count << " " << team_info[team_id].penalty << "\n";
                    fg = 0;
                    break;
                } else {
                    team_info[team_id] = tmp_info;
                }
            } else team_info[team_id].prob_status[i] =
                    team_info[team_id].freeze_prob_status[i];
        }
        if (fg) {
            if (it == board.begin()) break;
            else --it;
        }
    }
    PrintScoreBoard();
}

int main() {
    // 关闭同步，加速输入输出
    std::ios::sync_with_stdio(false);
    std::cout.tie(0);
    std::cin.tie(0);
    std::string blank; // 用于读入无用字符
    std::string op; // 用于读入操作
    while (std::cin >> op && op != "END") {
        if (op == "ADDTEAM") {
            std::string team_name;
            std::cin >> team_name;
            AddTeam(team_name);
        } else if (op == "START") {
            int duration_time, problem_count;
            std::cin >> blank >> duration_time >> blank >> problem_count;
            Start(duration_time, problem_count);
        } else if (op == "FLUSH") {
            Flush();
        } else if (op == "QUERY_RANKING") {
            std::string team_name;
            std::cin >> team_name;
            QueryRanking(team_name);
        } else if (op == "QUERY_SUBMISSION") {
            std::string team_name, prob_name, status;
            std::cin >> team_name >> blank >> prob_name >> blank >> status;
            QuerySubmission(team_name, prob_name.substr(8), GetStatus(status.substr(7)));
        } else if (op == "SUBMIT") {
            std::string prob_name, team_name, status;
            int time;
            std::cin >> prob_name >> blank >> team_name >> blank >> status >> blank >> time;
            Submit(prob_name, team_name, GetStatus(status), time);
        } else if (op == "FREEZE") {
            Freeze();
        } else if (op == "SCROLL") {
            Scroll();
        } else {
            std::cout << "[Error]Unknown operation.\n";
        }
    }
    std::cout << "[Info]Competition ends.\n";
    return 0;
}