//
// Created by german on 3/1/26.
//

#pragma once

#ifndef MYPROCESSMANAGER_CPUSNAPSHOT_H
#define MYPROCESSMANAGER_CPUSNAPSHOT_H

#include <string>
#include <unordered_map>

namespace myProc {
    /**
    * struct used to represent a snapshot of proc/stat values for cpu usage
    */
    struct CpuSnapShot {
        unsigned long user; //1 - time spent in user mode
        unsigned long nice; //2 - time spent in user mode with low priority
        unsigned long system; //3 - time spent in system mode
        unsigned long idle; //4 - time spent in idle tasks
        unsigned long iowait; //5 - time waiting fot I/O to complete
        unsigned long irq; //6 - time servicing interrupts
        unsigned long softirq; //7 - time servicing softirqs
        unsigned long steal; //8 - Time spent in SO when running virtual environtments
        unsigned long guest; //9 - Time spent running a virtual CPU for guest OS
        unsigned long guestNice; //10 - Time spent running a nice guest

        CpuSnapShot() {
        }

        /**
         * The constructor will cast the values directly from the file as string to long
         * @param user 1 - time spent in user mode
         * @param nice 2 - time spent in user mode with low priority
         * @param system 3 - time spent in system mode
         * @param idle 4 - time spent in idle tasks
         * @param iowait 5 - time waiting fot I/O to complete
         * @param irq 6 - time servicing interrupts
         * @param softirq 7 - time servicing softirqs
         * @param steal 8 - Time spent in SO when running virtual environtments
         * @param guest 9 - Time spent running a virtual CPU for guest OS
         * @param guestNice 10 - Time spent running a nice guest
         */
        CpuSnapShot(const std::string &user,
                    const std::string &nice,
                    const std::string &system,
                    const std::string &idle,
                    const std::string &iowait,
                    const std::string &irq,
                    const std::string &softirq,
                    const std::string &steal,
                    const std::string &guest,
                    const std::string &guestNice
        )
            : user(std::stoul(user)), nice(std::stoul(nice)),
              system(std::stoul(system)), idle(std::stoul(idle)),
              iowait(std::stoul(iowait)), irq(std::stoul(irq)),
              softirq(std::stoul(softirq)), steal(std::stoul(steal)),
              guest(std::stoul(guest)), guestNice(std::stoul(guestNice)) {
        }

        explicit CpuSnapShot(const std::unordered_map<std::string, std::string> &allData);

        [[nodiscard]] unsigned long get_user() const {
            return user;
        }

        void set_user(unsigned long user) {
            this->user = user;
        }

        [[nodiscard]] unsigned long get_nice() const {
            return nice;
        }

        void set_nice(unsigned long nice) {
            this->nice = nice;
        }

        [[nodiscard]] unsigned long get_system() const {
            return system;
        }

        void set_system(unsigned long system) {
            this->system = system;
        }

        [[nodiscard]] unsigned long get_idle() const {
            return idle;
        }

        void set_idle(unsigned long idle) {
            this->idle = idle;
        }

        [[nodiscard]] unsigned long get_iowait() const {
            return iowait;
        }

        void set_iowait(unsigned long iowait) {
            this->iowait = iowait;
        }

        [[nodiscard]] unsigned long get_irq() const {
            return irq;
        }

        void set_irq(unsigned long irq) {
            this->irq = irq;
        }

        [[nodiscard]] unsigned long get_softirq() const {
            return softirq;
        }

        void set_softirq(unsigned long softirq) {
            this->softirq = softirq;
        }

        [[nodiscard]] unsigned long get_steal() const {
            return steal;
        }

        void set_steal(unsigned long steal) {
            this->steal = steal;
        }

        [[nodiscard]] unsigned long get_guest() const {
            return guest;
        }

        void set_guest(unsigned long guest) {
            this->guest = guest;
        }

        [[nodiscard]] unsigned long get_guest_nice() const {
            return guestNice;
        }

        void set_guest_nice(unsigned long guest_nice) {
            guestNice = guest_nice;
        }
    };
}
#endif //MYPROCESSMANAGER_CPUSNAPSHOT_H
