/*
Copyright 2020 Google LLC

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    https://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#pragma once

#include <mutex>
#include <condition_variable>
#include <queue>
#include <thread>
#include <optional>
#include <variant>

namespace beehive {
class Message {
    public:
        enum class Kind {
            NOP,
            EXIT,
            TASK,
            DUMP,
        };

        struct NOP_Data {
            bool operator == (const NOP_Data& rhs) const { return true; }
        };
        struct EXIT_Data {
            bool operator == (const EXIT_Data& rhs) const { return true; }
        };
        struct TASK_Data {
            bool operator == (const TASK_Data& rhs) const { return true; }
        };
        struct DUMP_Data {
            bool operator == (const DUMP_Data& rhs) const { return true; }
        };

        Message(NOP_Data);
        Message(EXIT_Data);
        Message(TASK_Data);
        Message(DUMP_Data);

        Kind kind() const;

        std::optional<NOP_Data> nop() const;
        std::optional<EXIT_Data> exit() const;
        std::optional<TASK_Data> task() const;
        std::optional<DUMP_Data> dump() const;

        bool operator==(const Message& rhs) const;
        bool operator!=(const Message& rhs) const;
    private:

        Kind mKind;
        std::variant<NOP_Data, EXIT_Data, TASK_Data, DUMP_Data> mPayload;
};

class MessageQueue {
    public:
        MessageQueue();

        void send(Message);
        bool empty();
        std::optional<Message> receive();

    private:
        MessageQueue(const MessageQueue&) = delete;

        std::mutex mQueueMutex;
        std::queue<Message> mQueue;
};

class SignalingQueue {
    public:
        class Handler {
            public:
                enum class Result {
                    CONTINUE,
                    ERROR,
                    FINISH,
                };

                virtual void onBeforeMessage();
                virtual void onAfterMessage();

                virtual Result onNop(const Message::NOP_Data&);
                virtual Result onExit(const Message::EXIT_Data&);
                virtual Result onTask(const Message::TASK_Data&);
                virtual Result onDump(const Message::DUMP_Data&);
            protected:
                Handler();
                virtual ~Handler();
        };

        class HandlerThread : public Handler {
            public:
                HandlerThread();

                SignalingQueue* queue();

                void join();
                void detach();
            private:
                void loop();

                std::unique_ptr<SignalingQueue> mQueue;
                std::thread mThread;
        };

        SignalingQueue();

        void send(Message);
        Message receive();
    
        void loop(Handler*);

    private:
        MessageQueue mQueue;
        std::mutex mWaitMutex;
        std::condition_variable mWaitCV;
};
}
