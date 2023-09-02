#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define txt_name 50
#define max_server 5
// 監視できるサーバーの最大数
#define buffer_times 1
// 連続したタイムアウトを故障と認定するまでの回数（設問2）
#define react_times 2
// 平均応答時間を算出するためのデータ数（設問3）
#define react_limit 3
// 平均応答時間のしきい値[ms]（設問3）
#define num_monitor 12
// 監視するログの個数

int main() {
    int i, j, k;
    FILE *fp = NULL;

    char mon_log[txt_name] = "log_ex1.txt";
    // 監視するログファイル名をここに記入

    int y_store[max_server + 1][num_monitor + 1],
        m_store[max_server + 1][num_monitor + 1],
        d_store[max_server + 1][num_monitor + 1],
        h_store[max_server + 1][num_monitor + 1],
        min_store[max_server + 1][num_monitor + 1],
        sec_store[max_server + 1][num_monitor + 1],
        ping_store[max_server + 1][num_monitor + 1];

    for (j = 1; j < max_server + 1; j++) {
        for (i = 1; i < num_monitor + 1; i++) {
            y_store[j][i] = 0;
            m_store[j][i] = 0;
            d_store[j][i] = 0;
            h_store[j][i] = 0;
            min_store[j][i] = 0;
            sec_store[j][i] = 0;
            ping_store[j][i] = 0;
        }
    }

    int year, month, day, hour, min, sec, ping;
    int y_sub, m_sub, d_sub, h_sub, min_sub, sec_sub;
    int *y_err, *m_err, *d_err, *h_err, *min_err, *sec_err;
    y_err = (int *)malloc(sizeof(int) * (max_server + 1));
    m_err = (int *)malloc(sizeof(int) * (max_server + 1));
    d_err = (int *)malloc(sizeof(int) * (max_server + 1));
    h_err = (int *)malloc(sizeof(int) * (max_server + 1));
    min_err = (int *)malloc(sizeof(int) * (max_server + 1));
    sec_err = (int *)malloc(sizeof(int) * (max_server + 1));

    int counter_buffer[max_server + 1];
    for (i = 1; i < max_server + 1; i++) {
        counter_buffer[i] = 0;
    }
    int overload[max_server + 1][num_monitor + 1];

    char address[txt_name], ping_str[txt_name];
    char **ad_err;
    ad_err = (char **)malloc(sizeof(char *) * (max_server + 1));
    for (i = 1; i < max_server + 1; i++) {
        ad_err[i] = (char *)malloc(sizeof(char) * (txt_name));
    }
    for (i = 1; i < max_server + 1; i++) {
        strcpy(ad_err[i], "Not assigned");
    }  // 初期化

    fp = fopen(mon_log, "r");
    if (fp == NULL) {
        printf("---error:file could not be opend---\n");
        return -1;
    }

    for (i = 1; i < num_monitor + 1; i++) {
        fscanf(fp, "%4d%2d%2d%2d%2d%2d,", &year, &month, &day, &hour, &min,
               &sec);
        fscanf(fp, "%[^,],", address);
        fscanf(fp, "%s", &ping_str);
        if (*ping_str == '-') {
            ping = 1e+6;
        } else {
            ping = atoi(ping_str);
        }
        // 監視データの読み込み

        for (j = 1; j < max_server + 1; j++) {
            if (strcmp(ad_err[j], "Not assigned") == 0) {
                strcpy(ad_err[j], address);
                break;
            } else if (strcmp(address, ad_err[j]) == 0) {
                break;
            }
        }  // サーバーアドレスの保存

        for (j = 1; j < max_server + 1; j++) {
            if (strcmp(address, ad_err[j]) == 0) {
                y_store[j][i] = year;
                m_store[j][i] = month;
                d_store[j][i] = day;
                h_store[j][i] = hour;
                min_store[j][i] = min;
                sec_store[j][i] = sec;
                ping_store[j][i] = ping;
            }
        }  // 各データの格納

        if (ping == 1e+6) {
            // pingのタイムアウトを検知
            for (j = 1; j < max_server + 1; j++) {
                if (strcmp(address, ad_err[j]) == 0) {
                    if (counter_buffer[j] == 0) {
                        y_err[j] = year;
                        m_err[j] = month;
                        d_err[j] = day;
                        h_err[j] = hour;
                        min_err[j] = min;
                        sec_err[j] = sec;
                        // タイムアウト時のアドレス，日時を保存
                        counter_buffer[j] += 1;
                        break;
                    } else {
                        counter_buffer[j] += 1;
                        break;
                        // タイムアウトが継続するときの処理;
                    }
                }
            }
        } else {
            for (j = 1; j < max_server + 1; j++) {
                if (strcmp(address, ad_err[j]) == 0) {
                    if (counter_buffer[j] >= buffer_times) {
                        printf("\n---error detected!---\n");
                        printf(" server:%s\n", address);
                        y_sub = year - y_err[j];
                        m_sub = month - m_err[j];
                        d_sub = day - d_err[j];
                        h_sub = hour - h_err[j];
                        min_sub = min - min_err[j];
                        sec_sub = sec - sec_err[j];
                        // タイムアウト期間の計算
                        if (sec_sub < 0) {
                            min_sub -= 1;
                            sec_sub += 60;
                        }
                        if (min_sub < 0) {
                            h_sub -= 1;
                            min_sub += 60;
                        }
                        if (h_sub < 0) {
                            d_sub -= 1;
                            h_sub += 24;
                        }
                        if (d_sub < 0) {
                            m_sub -= 1;
                            d_sub += 30;
                        }
                        if (m_sub < 0) {
                            y_sub -= 1;
                            m_sub += 365;
                        }
                        // 時間の計算の処理
                        printf(" TBF(y-m-d)=(%04d/%02d/%02d)\n", y_sub, m_sub,
                               d_sub);

                        printf("    (h:m:s)=(%02d:%02d:%02d)\n", h_sub, min_sub,
                               sec_sub);
                    }
                    y_err[j] = -99999;
                    m_err[j] = -999;
                    d_err[j] = -999;
                    h_err[j] = -999;
                    min_err[j] = -999;
                    sec_err[j] = -999;
                    counter_buffer[j] = 0;
                    // 初期化
                    break;
                }
            }
        }
    }

    int y_be, m_be, d_be, h_be, min_be, sec_be;
    double ping_ave = 0.0, counter_OL;

    for (i = 1; i < num_monitor + 1; i++) {
        for (j = 1; j < max_server + 1; j++) {
            if (((y_store[j][i] == 0) || (m_store[j][i] == 0) ||
                 (d_store[j][i] == 0) || (h_store[j][i] == 0) ||
                 (min_store[j][i] == 0) || (sec_store[j][i] == 0))) {
                overload[j][i] = -1;
            } else {
                overload[j][i] = 0;
            }
        }
    }  // 過負荷検知に用いらない配列部の除外処理

    for (j = 1; j < max_server + 1; j++) {
        if (strcmp(ad_err[j], "Not assigned") != 0) {
            for (i = 1; i < num_monitor + 1; i++) {
                if (overload[j][i] != -1) {
                    ping_ave = 0.0;
                    counter_OL = react_times;
                    for (k = i; k > 0; k--) {
                        if (overload[j][k] != -1) {
                            ping_ave += (double)ping_store[j][k];
                            counter_OL -= 1;
                        }
                        if (counter_OL <= 0) {
                            break;
                        }
                    }
                    if ((ping_ave / react_times) > react_limit) {
                        overload[j][i] = 1;
                    }
                }
            }
        }
    }

    for (j = 1; j < max_server + 1; j++) {
        y_be = 0;
        m_be = 0;
        d_be = 0;
        h_be = 0;
        min_be = 0;
        sec_be = 0;
        // 初期化

        if (strcmp(ad_err[j], "Not assigned") != 0) {
            for (i = 1; i < num_monitor + 1; i++) {
                if ((overload[j][i] == 1)) {
                    if ((y_be == 0) && (m_be == 0) && (d_be == 0) &&
                        (h_be == 0) && (min_be == 0) && (sec_be == 0)) {
                        y_be = y_store[j][i];
                        m_be = m_store[j][i];
                        d_be = d_store[j][i];
                        h_be = h_store[j][i];
                        min_be = min_store[j][i];
                        sec_be = sec_store[j][i];
                    }  // 過負荷検知直後の時間を保存
                } else {
                    if ((overload[j][i] != -1) &&
                        ((y_be != 0) || (m_be != 0) || (d_be != 0) ||
                         (h_be != 0) || (min_be != 0) || (sec_be != 0))) {
                        printf("\n---server overload detected---\n");
                        printf(" server :%s\n", ad_err[j]);
                        printf("          before         till\n", ad_err[j]);
                        printf(
                            "(y-m-d)=(%04d/%02d/%02d)~(%04d/%02d/"
                            "%02d)\n",
                            y_be, m_be, d_be, y_store[j][i], m_store[j][i],
                            d_store[j][i]);
                        printf(
                            "(h:m:s)=(%02d:%02d:%02d)~(%02d:%02d:%"
                            "02d)\n",
                            h_be, min_be, sec_be, h_store[j][i],
                            min_store[j][i], sec_store[j][i]);
                        y_err[j] = -99999;
                        m_err[j] = -999;
                        d_err[j] = -999;
                        h_err[j] = -999;
                        min_err[j] = -999;
                        sec_err[j] = -999;
                        y_be = 0;
                        m_be = 0;
                        d_be = 0;
                        h_be = 0;
                        min_be = 0;
                        sec_be = 0;
                        overload[j][i] = 0;
                        // 保存した情報の初期化
                    }
                }
            }
        }
    }

    fclose(fp);
    free(ad_err);
    free(y_err);
    free(m_err);
    free(d_err);
    free(h_err);
    free(min_err);
    free(sec_err);

    return 0;
}