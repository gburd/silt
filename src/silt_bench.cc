/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*- */

/*-
 * SILT: A Memory-Efficient, High-Performance Key-Value Store
 *
 * Copyright © 2011 Carnegie Mellon University
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License.  You may obtain a copy
 * of the License at: http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 * IN NO EVENT SHALL THE AUTHORS OR DISTRIBUTORS BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
 * OF THE USE OF THIS SOFTWARE, ITS DOCUMENTATION, OR ANY DERIVATIVES THEREOF,
 * EVEN IF THE AUTHORS HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * THE AUTHORS AND DISTRIBUTORS SPECIFICALLY DISCLAIM ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, AND NON-INFRINGEMENT. THIS SOFTWARE IS
 * PROVIDED ON AN "AS IS" BASIS, AND THE AUTHORS AND DISTRIBUTORS HAVE NO
 * OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR
 * MODIFICATIONS.
 *
 * GOVERNMENT USE: If you are acquiring this software on behalf of the
 * U.S. government, the Government shall have only "Restricted Rights" in the
 * software and related documentation as defined in the Federal Acquisition
 * Regulations (FARs) in Clause 52.227.19 (c) (2). If you are acquiring the
 * software on behalf of the Department of Defense, the software shall be
 * classified as "Commercial Computer Software" and the Government shall have
 * only "Restricted Rights" as defined in Clause 252.227-7013 (c) (1) of
 * DFARs. Notwithstanding the foregoing, the authors grant the U.S. Government
 * and others acting in its behalf permission to use and distribute the
 * software in accordance with the terms specified in this license.
 */

#include "silt_factory.h"
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#include "print.h"
#include "dbid.h"
#include "hashutil.h"
#include "timing.h"

enum { SCAN_RANDOM, SCAN_SEQUENTIAL };

using namespace std;
using namespace silt;
using silt::HashUtil;

struct benchData {
    Silt **f;
    u_int num_records;
    u_int num_to_scan;
    u_int offset;
    u_int numThreads;
};

volatile uint32_t readyCount = 0;
pthread_mutex_t count_lock;
vector<double> search_times;
u_int max_record = 0;


void
usage()
{
    fprintf(stderr,
            "silt_bench [-hfsw] [-r num_scan] [-n num_rec] [-c \"config1 config2 config3\"] [-a num_procs]\n"
           );
}

void
help()
{
    usage();
    fprintf(stderr,
            "   -h      help (this text)\n"
            "   -f      fill\n"
            "   -s      sequential scan\n"
            "   -r #    number of entries to randomly scan\n"
            "   -n #    number of entries to fill or scan.\n"
            "   -c #    config files for one or more threads\n"
            "   -a #    Turn on thread affinity, specify number of processors\n"
            "   -w      random write test\n"
            "   -S #    set value size (bytes)\n"
           );
}

void *randomReadThread(void *p)
{
    benchData *bd = (benchData *)p;
    Silt **dbs = bd->f;
    u_int num_records = bd->num_records;
    u_int num_to_scan = bd->num_to_scan;
    u_int offsetVal = bd->offset;
    Silt *mydb = dbs[offsetVal];
    struct timeval tv_start, tv_end;
    char *l = (char *)malloc(sizeof(char) * num_to_scan * sizeof(uint32_t));

    for (u_int i = 0; i < num_to_scan; i++) {
        u_int val = (offsetVal * num_records) + (rand() % num_records);

        if (val < max_record) {
            string ps_key((const char *)&val, sizeof(val));
            uint32_t key_id = HashUtil::BobHash(ps_key);
            DBID key((char *)&key_id, sizeof(u_int32_t));
            memcpy(l + (i * sizeof(uint32_t)), key.data(), key.get_actual_size());
        } else {
            i--;
        }
    }

    pthread_mutex_lock(&count_lock);
    readyCount++;
    pthread_mutex_unlock(&count_lock);
    struct timespec req;
    req.tv_sec = 0;
    req.tv_nsec = 20000;

    while (readyCount < bd->numThreads) {
        nanosleep(&req, NULL);
    }

    gettimeofday(&tv_start, NULL);
    //char* data;
    //uint32_t data_len;
    //std::string data;
    Value data;
    const char *key = l;

    //bool callerDeletes = true;
    for (u_int i = 0; i < num_to_scan; ++i) {
        if (mydb->Get(ConstRefValue(key, sizeof(uint32_t)), data) != OK) {
            perror("Get failed.\n");
        }

        key += sizeof(uint32_t);
    }

    gettimeofday(&tv_end, NULL);
    double dbsearch_time = timeval_diff(&tv_start, &tv_end);
    //printf("Time to search DB: %f seconds\n", dbsearch_time);
    //printf("Query rate: %f\n", ((double)num_to_scan / dbsearch_time) );
    free(l);
    pthread_mutex_lock(&count_lock);
    search_times.push_back(dbsearch_time);
    pthread_mutex_unlock(&count_lock);
    return NULL;
}

void bench(int argc, char **argv)
{
    extern char *optarg;
    extern int optind;
    int ch;
    vector<string> fileBases;
    u_int num_to_scan = 0;
    int mode = SCAN_RANDOM;
    bool createdb = false;
    int writeTest = 0;
    int numThreads = 1;
    bool useThreads = false;
    bool setAffinity = false;
    int numProcs = 1;
    int valuesize = 1024;

    while ((ch = getopt(argc, argv, "hfn:r:p:swc:a:S:")) != -1)
        switch (ch) {
            case 'n':
                max_record = atoi(optarg);
                break;

            case 'r':
                num_to_scan = atoi(optarg);
                break;

            case 'f':
                createdb = true;
                break;

            case 's':
                mode = SCAN_SEQUENTIAL;
                break;

            case 'w':
                writeTest = 1;
                break;

            case 'c':
                tokenize(optarg, fileBases, " ");
                useThreads = true;
                break;

            case 'S':
                valuesize = atoi(optarg);
                break;

            case 'a':
                setAffinity = true;
                numProcs = atoi(optarg);
                break;

            case 'h':
                help();
                exit(0);

            default:
                usage();
                exit(-1);
        }

    argc -= optind;
    argv += optind;

    if (fileBases.size() == 0 || argc != 0) {
        usage();
        exit(-1);
    }

    numThreads = fileBases.size();
    struct timeval tv_start, tv_end;
    gettimeofday(&tv_start, NULL);
    //char key[4];
    //char data[valuesize];
    //string value(valuesize, 'a');
    Silt **dbs = (Silt **)malloc(numThreads * sizeof(Silt *));
    // size?  num_records / numThreads
    int num_recs_per_db = (int)(max_record / numThreads);
    int bucket = num_recs_per_db;

    if (max_record % numThreads != 0)
        bucket += 1;

    pthread_t *workerThreadIds_ = (pthread_t *) malloc(numThreads * sizeof(pthread_t));
    benchData *bd = (benchData *) malloc(numThreads * sizeof(benchData));

    //u_int fileBaseOffset = 0;
    for (u_int i = 0; i < fileBases.size(); i++) {
        ostringstream dbname_i;
        dbname_i << fileBases[i];

        if (createdb) {
            //TODO: Put size value inside config file!
            // uint64_t size = num_recs_per_db * 2;
            dbs[i] = Silt_Factory::New(dbname_i.str().c_str()); // .9, .8, htstore
            dbs[i]->Create();
        } else {
            printf("opening silt with config file %s\n", dbname_i.str().c_str());
            dbs[i] = Silt_Factory::New(dbname_i.str().c_str()); // htstore
            dbs[i]->Open();
        }
    }

    if (createdb && !writeTest) {
        // Fill it sequentially if we're not testing writing
        for (u_int i = 0; i < max_record; ++i) {
            int num = i;
            string ps_key((const char *)&num, sizeof(num));
            u_int32_t key_id = HashUtil::BobHash(ps_key);
            DBID key((char *)&key_id, sizeof(u_int32_t));
            int dbi = (int)(i / bucket);
            Value data;

            if (dbs[dbi]->Put(ConstRefValue(key.data(), sizeof(uint32_t)), data) != OK) {
                perror("Insert failed\n");
            }
        }

        // this is required since we're not splitting/merging/rewriting initially
        for (int i = 0; i < numThreads; i++) {
            if (dbs[i]->Flush()) {
                perror("Could not flush Silt.\n");
            }
        }
    }

    gettimeofday(&tv_end, NULL);

    if (createdb) {
        double dbcreate_time = timeval_diff(&tv_start, &tv_end);
        printf("Time to create DB: %f seconds\n", dbcreate_time);
    }

    srand((tv_end.tv_sec << 2) + tv_end.tv_usec);

    // Set of randomly ordered test elements

    //random_shuffle(l.begin(), l.end());

    if (writeTest) {
        vector<int> l;

        for (u_int i = 0; i < num_to_scan; i++) {
            l.push_back(rand() % max_record);
        }

        int n = l.size();

        for (int i = 0; i < n; i++) {
            u_int val = l[i];
            string ps_key((const char *)&val, sizeof(val));
            u_int32_t key_id = HashUtil::BobHash(ps_key);
            DBID key((char *)&key_id, sizeof(u_int32_t));
            int dbi = (int)(i / bucket);
            Value data;

            if (dbs[dbi]->Get(ConstRefValue(key.data(), sizeof(uint32_t)), data) != OK) {
                perror("Insert failed\n");
            }
        }
    } else {
        pthread_mutex_init(&count_lock, NULL);

        for (int i = 0; i < numThreads; i++) {
            bd[i].f = dbs;
            bd[i].num_to_scan = num_to_scan;
            bd[i].num_records = bucket;
            bd[i].offset = i;
            bd[i].numThreads = numThreads;

            if (useThreads) {
                pthread_attr_t attr;
                pthread_attr_init(&attr);
#ifdef cpu_set_t /* GNU/Linux-only! */

                if (setAffinity) {
                    cpu_set_t cpuset;
                    CPU_ZERO(&cpuset);
                    CPU_SET(i % numProcs, &cpuset);
                    pthread_attr_setaffinity_np(&attr, sizeof(cpuset), &cpuset);
                }

#endif
                pthread_create(&workerThreadIds_[i], &attr,
                        randomReadThread, &bd[i]);
            } else {
                randomReadThread(&bd[0]);
            }
        }
    }

    if (useThreads) {
        for (int i = 0; i < numThreads; i++) {
            pthread_join(workerThreadIds_[i], NULL);
        }
    }

    pthread_mutex_destroy(&count_lock);
    free(workerThreadIds_);
    free(bd);
    double totalTime = 0;

    for (int i = 0; i < numThreads; i++) {
        totalTime = max(totalTime, search_times[i]);
    }

    double totalQueries = num_to_scan * numThreads;
    cout << "Aggregate Query Rate: " << totalQueries / totalTime << " queries per second" << endl;
}


int main(int argc, char **argv)
{
    bench(argc, argv);
    return 0;
}
