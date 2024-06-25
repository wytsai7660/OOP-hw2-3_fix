#include <climits>
#include <concepts>
#include <cstddef>
#include <functional>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <queue>
#include <set>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

// The double parentheses in decltype are significant.
#define SET(var_name) \
    template <typename T> \
    void set_##var_name(T &&_##var_name) { \
        static_assert(std::is_assignable_v<decltype((var_name)), T>); \
        (var_name) = std::forward<T>(_##var_name); \
    }

#define SET_WITH_NAME(setter_name, var_name) \
    template <typename T> \
    void setter_name(T &&_##var_name) { \
        static_assert(std::is_assignable_v<decltype((var_name)), T>); \
        (var_name) = std::forward<T>(_##var_name); \
    }

#define GET(var_name) \
    const auto &get_##var_name() const & { return var_name; } \
    auto get_##var_name() && { return std::move(var_name); }

#define GET_WITH_NAME(getter_name, var_name) \
    const auto &getter_name() const & { return var_name; } \
    auto getter_name() && { return std::move(var_name); }

/*
This is unreliable. The static constructor seems to only be called in
non-template classes, including non-template subclasses of template classes.
Even then, the standard doesn't even guarantee that it will be called in a
non-template classes.
*/
#define STATIC_CONSTRUCTOR(body) STATIC_CONSTRUCTOR_IMPL1(body, __COUNTER__)

#define STATIC_CONSTRUCTOR_IMPL1(body, counter) STATIC_CONSTRUCTOR_IMPL2(body, counter) // Expands __COUNTER__

#define STATIC_CONSTRUCTOR_IMPL2(body, counter) \
    class static_constructor##counter { \
        public: \
            static_constructor##counter() { \
                body \
            } \
    }; \
    static inline static_constructor##counter static_constructor;

#define DEFAULTED_SPECIAL_MEMBERS(class_name) \
    virtual ~class_name() = default; \
    DEFAULTED_SPECIAL_MEMBERS_WITHOUT_DESTRUCTOR(class_name)

#define DEFAULTED_SPECIAL_MEMBERS_WITHOUT_DESTRUCTOR(class_name) \
    class_name() = default; \
    class_name(const class_name &other) = default; \
    class_name(class_name &&other) = default; \
    class_name &operator=(const class_name &other) = default; \
    class_name &operator=(class_name &&other) = default;

template<typename... Ts>
struct overloaded : Ts... { using Ts::operator()...; };

class header;
class payload;

template <std::derived_from<header> HeaderType, std::derived_from<payload> PayloadType, typename Derived>
class packet;

class node;
class event;
class link; // new

// for simplicity, we use a const int to simulate the delay
// if you want to simulate the more details, you should revise it to be a class
const unsigned int ONE_HOP_DELAY = 10;
const unsigned int BROADCAST_ID = UINT_MAX;

// BROADCAST_ID means that all neighbors are receivers; UINT_MAX is the maximum value of unsigned int

class header {
    public:
        virtual ~header() = default;

        SET(src_ID)
        SET(dst_ID)
        SET(pre_ID)
        SET(nex_ID)
        GET(src_ID)
        GET(dst_ID)
        GET(pre_ID)
        GET(nex_ID)
    virtual std::string type() = 0;

        static void print () {
            std::cout << "registered header types:\n";
            for (const auto &name: derived_class_names) {
                std::cout << name << '\n';
            }
        }

    protected:
        DEFAULTED_SPECIAL_MEMBERS_WITHOUT_DESTRUCTOR(header)
        static inline std::vector<std::string> derived_class_names;

    private:
        unsigned int src_ID = BROADCAST_ID;
        unsigned int dst_ID = BROADCAST_ID;
        unsigned int pre_ID = BROADCAST_ID;
        unsigned int nex_ID = BROADCAST_ID;
};

class IoT_data_header : public header{
        STATIC_CONSTRUCTOR (
            derived_class_names.emplace_back("IoT_data_header");
        )

    public:
        std::string type() override { return "IoT_data_header"; }
};

class IoT_ctrl_header : public header{
        STATIC_CONSTRUCTOR (
            derived_class_names.emplace_back("IoT_ctrl_header");
        )

    public:
        std::string type() override { return "IoT_ctrl_header"; }
};

class AGG_ctrl_header : public header{
        STATIC_CONSTRUCTOR (
            derived_class_names.emplace_back("AGG_ctrl_header");
        )

    public:
        std::string type() override { return "AGG_ctrl_header"; }
};

class DIS_ctrl_header : public header{
        STATIC_CONSTRUCTOR (
            derived_class_names.emplace_back("DIS_ctrl_header");
        )

    public:
        std::string type() override { return "DIS_ctrl_header"; }
};

class payload {
        std::string msg;

    protected:
        DEFAULTED_SPECIAL_MEMBERS_WITHOUT_DESTRUCTOR(payload)
        static inline std::vector<std::string> derived_class_names;
    public:
        virtual ~payload() = default;

        virtual std::string type() = 0;

        SET(msg)
        GET(msg)

        static void print () {
            std::cout << "registered payload types:\n";
            for (const auto &name: derived_class_names) {
                std::cout << name << '\n';
            }
        }
};

class IoT_data_payload : public payload {
        STATIC_CONSTRUCTOR (
            derived_class_names.emplace_back("IoT_data_payload");
        )

    public:
        std::string type() override { return "IoT_data_payload"; }
};

class IoT_ctrl_payload : public payload {
        STATIC_CONSTRUCTOR (
            derived_class_names.emplace_back("IoT_ctrl_payload");
        )

        unsigned int counter = 0;
    public:
        void increase() { counter ++; } // used to increase the counter
        GET(counter) // used to get the value of counter

        std::string type() override { return "IoT_ctrl_payload"; }
};

class AGG_ctrl_payload : public payload {
        STATIC_CONSTRUCTOR (
            derived_class_names.emplace_back("AGG_ctrl_payload");
        )

    // unsigned int counter ;

    public:
        // void increase() { counter ++; } // used to increase the counter
        // GET(getCounter,unsigned int,counter); // used to get the value of counter

        std::string type() override { return "AGG_ctrl_payload"; }
};

class DIS_ctrl_payload : public payload {
        STATIC_CONSTRUCTOR (
            derived_class_names.emplace_back("DIS_ctrl_payload");
        )

        // unsigned int counter ;
        unsigned int parent = 0;

    public:
        // void increase() { counter ++; } // used to increase the counter
        SET(parent)
        GET(parent) // used to get the value of counter

        explicit DIS_ctrl_payload(unsigned int _parent = 0): parent (_parent) {}
        std::string type() override { return "DIS_ctrl_payload"; }
};

class packet_derived_classes_common_fields_holder {
    protected:
        static inline std::vector<std::string> derived_class_names;
        static inline unsigned int last_packet_id;
        static inline unsigned int live_packet_num;
};

template <std::derived_from<header> HeaderType, std::derived_from<payload> PayloadType, typename Derived>
class packet : protected packet_derived_classes_common_fields_holder {
        // a packet usually contains a header and a payload
        HeaderType hdr;
        PayloadType pld;
        unsigned int p_id;
        using packet_derived_classes_common_fields_holder::last_packet_id;
        using packet_derived_classes_common_fields_holder::live_packet_num;
    protected:
        PayloadType &get_payload_non_const() {
            return pld;
        }
        packet(): p_id(last_packet_id++) {
            live_packet_num++;
        }
        packet(const packet &other) : hdr(other.hdr), pld(other.pld), p_id(other.p_id) {
            live_packet_num++;
        }
        /*
        The move operations does the same thing as the copy operations.

        The reason why the move operations also increment live_packet_num,
        unlike shared_ptr, is because shared_ptr is thread-safe, so incrementing
        and decrementing the reference count is time-consuming, but here,
        live_packet_num isn't thread-safe in the first place, so it becomes
        pointless to do so.
        */
        packet(packet &&other) noexcept : hdr(std::move(other.hdr)), pld(std::move(other.pld)), p_id(std::move(other.p_id)) {
            live_packet_num++;
        }
        packet &operator=(const packet &other) {
            Derived temp(*static_cast<const Derived *>(&other)); // Derived must be a subclass so static_cast will do.
            swap(*this, temp);
            return *this;
        }
        packet &operator=(packet &&other) noexcept {
            (void)std::move(other); // Silences "not moved from" warning
            swap(*this, other);
            return *this;
        }
        template <typename DeducedHeaderType, typename DeducedPayloadType>
        requires std::same_as<std::remove_cvref_t<DeducedHeaderType>, HeaderType> &&
            std::same_as<std::remove_cvref_t<DeducedPayloadType>, PayloadType>
        packet(DeducedHeaderType &&_hdr, DeducedPayloadType &&_pld, bool rep = false, unsigned int rep_id = 0) : hdr(std::forward<DeducedHeaderType>(_hdr)), pld(std::forward<DeducedPayloadType>(_pld)) {
            if (! rep )  { // a duplicated packet does not have a new packet id
                p_id = last_packet_id ++;
            }
            else {
                p_id = rep_id;
            }
            live_packet_num ++;
        }
    public:
        virtual ~packet(){
            // cout << "packet destructor begin" << '\n';
            live_packet_num --;
            // cout << "packet destructor end" << '\n';
        }
        // This is friend so that ADL can find it.
        friend void swap(packet &first, packet &second) noexcept {
            using std::swap;
            swap(first.hdr, second.hdr);
            swap(first.pld, second.pld);
            swap(first.p_id, second.p_id);
        }
        SET_WITH_NAME(set_header, hdr)
        GET_WITH_NAME(get_header, hdr)
        SET_WITH_NAME(set_payload, pld)
        GET_WITH_NAME(get_payload, pld)
        GET_WITH_NAME(get_packet_ID, p_id)

        virtual std::string type () const = 0;
        // you can define your own packet's addition_information
        // to print more information for recv_event and send_event
        virtual std::string addition_information () const { return ""; }

        static unsigned int get_live_packet_num () { return live_packet_num; }

        static void print () {
            std::cout << "registered packet types:\n";
            for (const auto &name: derived_class_names) {
                std::cout << name << '\n';
            }
        }

        void set_src_ID(unsigned int id) {
            hdr.set_src_ID(id);
        }

        void set_dst_ID(unsigned int id) {
            hdr.set_dst_ID(id);
        }

        void set_pre_ID(unsigned int id) {
            hdr.set_pre_ID(id);
        }

        void set_nex_ID(unsigned int id) {
            hdr.set_nex_ID(id);
        }

        void set_msg(const std::string &msg) {
            pld.set_msg(msg);
        }
};

// this packet is used to transmit the data
class IoT_data_packet: public packet<IoT_data_header, IoT_data_payload, IoT_data_packet> {
        STATIC_CONSTRUCTOR (
            derived_class_names.emplace_back("IoT_data_packet");
        )

    public:
        IoT_data_packet() = default;
        
        template <typename DeducedPacketType>
        requires std::same_as<std::remove_cvref_t<DeducedPacketType>, packet>
        explicit IoT_data_packet(DeducedPacketType &&other) : packet(std::forward<DeducedPacketType>(other).get_header(), std::forward<DeducedPacketType>(other).get_payload(), true, other.get_packet_ID()) {}
        
        template <typename DeducedHeaderType, typename DeducedPayloadType>
        requires std::same_as<std::remove_cvref_t<DeducedHeaderType>, IoT_data_header> &&
            std::same_as<std::remove_cvref_t<DeducedPayloadType>, IoT_data_payload>
        IoT_data_packet(DeducedHeaderType &&header, DeducedPayloadType &&payload) : packet(std::forward<DeducedHeaderType>(header), std::forward<DeducedPayloadType>(payload)) {}
        
        std::string type() const override { return "IoT_data_packet"; }
};

// this packet type is used to conduct distributed BFS
class IoT_ctrl_packet: public packet<IoT_ctrl_header, IoT_ctrl_payload, IoT_ctrl_packet> {
        STATIC_CONSTRUCTOR (
            derived_class_names.emplace_back("IoT_ctrl_packet");
        )

    public:
        IoT_ctrl_packet() = default;
        
        template <typename DeducedPacketType>
        requires std::same_as<std::remove_cvref_t<DeducedPacketType>, packet>
        explicit IoT_ctrl_packet(DeducedPacketType &&other) : packet(std::forward<DeducedPacketType>(other).get_header(), std::forward<DeducedPacketType>(other).get_payload(), true, other.get_packet_ID()) {}
        
        template <typename DeducedHeaderType, typename DeducedPayloadType>
        requires std::same_as<std::remove_cvref_t<DeducedHeaderType>, IoT_ctrl_header> &&
            std::same_as<std::remove_cvref_t<DeducedPayloadType>, IoT_ctrl_payload>
        IoT_ctrl_packet(DeducedHeaderType &&header, DeducedPayloadType &&payload) : packet(std::forward<DeducedHeaderType>(header), std::forward<DeducedPayloadType>(payload)) {}

        std::string type() const override { return "IoT_ctrl_packet"; }
        std::string addition_information() const override {
            unsigned int counter = get_payload().get_counter();
            // cout << counter << '\n';
            return " counter " + std::to_string(counter);
        }
        void increase_payload_counter() {
            get_payload_non_const().increase();
        }
};

// this packet type is used to transmit each device's nblist to the sink
class AGG_ctrl_packet: public packet<AGG_ctrl_header, AGG_ctrl_payload, AGG_ctrl_packet> {
        STATIC_CONSTRUCTOR (
            derived_class_names.emplace_back("AGG_ctrl_packet");
        )

    public:
        AGG_ctrl_packet() = default;
        
        template <typename DeducedPacketType>
        requires std::same_as<std::remove_cvref_t<DeducedPacketType>, packet>
        explicit AGG_ctrl_packet(DeducedPacketType &&other) : packet(std::forward<DeducedPacketType>(other).get_header(), std::forward<DeducedPacketType>(other).get_payload(), true, other.get_packet_ID()) {}
        
        template <typename DeducedHeaderType, typename DeducedPayloadType>
        requires std::same_as<std::remove_cvref_t<DeducedHeaderType>, AGG_ctrl_header> &&
            std::same_as<std::remove_cvref_t<DeducedPayloadType>, AGG_ctrl_payload>
        AGG_ctrl_packet(DeducedHeaderType &&header, DeducedPayloadType &&payload) : packet(std::forward<DeducedHeaderType>(header), std::forward<DeducedPayloadType>(payload)) {}

        std::string type() const override { return "AGG_ctrl_packet"; }

        // virtual string addition_information() {
        //     string msg = (dynamic_cast<AGG_ctrl_payload*>(this->get_payload()))->getMsg();
        //     return " msg " + msg;
        // }
};

// this packet type is used to transmit the new parent to each device
class DIS_ctrl_packet: public packet<DIS_ctrl_header, DIS_ctrl_payload, DIS_ctrl_packet> {
        STATIC_CONSTRUCTOR (
            derived_class_names.emplace_back("DIS_ctrl_packet");
        )

    public:
        DIS_ctrl_packet() = default;
        
        template <typename DeducedPacketType>
        requires std::same_as<std::remove_cvref_t<DeducedPacketType>, packet>
        explicit DIS_ctrl_packet(DeducedPacketType &&other) : packet(std::forward<DeducedPacketType>(other).get_header(), std::forward<DeducedPacketType>(other).get_payload(), true, other.get_packet_ID()) {}
        
        template <typename DeducedHeaderType, typename DeducedPayloadType>
        requires std::same_as<std::remove_cvref_t<DeducedHeaderType>, DIS_ctrl_header> &&
            std::same_as<std::remove_cvref_t<DeducedPayloadType>, DIS_ctrl_payload>
        DIS_ctrl_packet(DeducedHeaderType &&header, DeducedPayloadType &&payload) : packet(std::forward<DeducedHeaderType>(header), std::forward<DeducedPayloadType>(payload)) {}

        std::string type() const override { return "DIS_ctrl_packet"; }
        std::string addition_information() const override {
            unsigned int parent = get_payload().get_parent();
            // cout << counter << '\n';
            return " parent " + std::to_string(parent);
        }

        void set_parent(unsigned int parent) {
            get_payload_non_const().set_parent(parent);
        }
};

class node {
        // all nodes created in the program
        static inline std::map<unsigned int, std::shared_ptr<node>> id_node_table;
        unsigned int id;
        std::set<unsigned int> phy_neighbors;

    protected:
        static inline std::vector<std::string> derived_class_names;
        explicit node(unsigned int _id): id(_id) {
            if(id_node_table.find(_id) != id_node_table.end()){
                throw std::invalid_argument("Duplicate node id");
            }
            if ( BROADCAST_ID == _id ) {
                throw std::invalid_argument("BROADCAST_ID cannot be used");
            }
        }
        static void register_node(const std::shared_ptr<node> &node) {
            id_node_table[node->id] = node;
        }

    public:
        /*
        When a node destructs, it removes itself from id_node_table, so it will
        be troublesome if a node is copyable because it will then try to remove
        itself twice.

        Although technically move operations are possible to implement, it
        wasn't used in the original program, so they are also deleted for
        simplicity.
        */
        node() = delete;
        node(const node &other) = delete;
        node(node &&other) = delete;
        node &operator=(const node &other) = delete;
        node &operator=(node &&other) = delete;
        virtual ~node() = default; // erase the node
        virtual std::string type() = 0; // please define it in your derived node class

        void add_phy_neighbor (unsigned int _id); // we only add a directed link from id to _id
        void del_phy_neighbor (unsigned int _id) { // we only delete a directed link from id to _id
            phy_neighbors.erase(_id);
        }

        // you can use the function to get the node's neighbors at this time
        // but in the project 3, you are not allowed to use this function
        const std::set<unsigned int> &get_phy_neighbors() { return phy_neighbors; }

        using PacketTypes = std::variant<std::monostate, IoT_ctrl_packet, IoT_data_packet, AGG_ctrl_packet, DIS_ctrl_packet>;

        void recv (PacketTypes &p) {
            recv_handler(p);
        } // the packet will be directly deleted after the handler
        void send (PacketTypes &p);

        // receive the packet and do something; this is a pure virtual function
        virtual void recv_handler(PacketTypes &p) = 0;
        static void send_handler(PacketTypes &p);

        static std::shared_ptr<node> id_to_node (unsigned int _id) {
            const auto it = id_node_table.find(_id);
            return it != id_node_table.cend() ? it->second : nullptr;
        }
        GET_WITH_NAME(get_node_ID, id)

        static void del_node (unsigned int _id) {
            const auto it = id_node_table.find(_id);
            if (it != id_node_table.cend()) {
                id_node_table.erase(it);
            }
        }
        static auto get_node_num () { return id_node_table.size(); }

        static void print () {
            std::cout << "registered node types:\n";
            for (const auto &name: derived_class_names) {
                std::cout << name << '\n';
            }
        }
};

class has_parent {
    public:
        virtual unsigned int get_parent_id() const = 0;
        virtual ~has_parent() = default;
    protected:
        DEFAULTED_SPECIAL_MEMBERS_WITHOUT_DESTRUCTOR(has_parent)
};

class IoT_device: public node, public has_parent {
        // map<unsigned int,bool> one_hop_neighbors; // you can use this variable to record the node's 1-hop neighbors
        STATIC_CONSTRUCTOR (
            derived_class_names.emplace_back("IoT_device");
        )

        bool hi = false; // this is used for example; you can remove it when doing hw2
        unsigned int parent_id = 0;

        explicit IoT_device(unsigned int _id): node(_id) {}
    public:
        unsigned int get_parent_id() const override {
            return parent_id;
        }
        static std::shared_ptr<IoT_device> generate(unsigned int _id) {
            try {
                std::shared_ptr<IoT_device> device(new IoT_device(_id));
                register_node(device);
                return device;
            }
            catch (...) {
                throw;
            }
        }
        std::string type() override { return "IoT_device"; }

        // please define recv_handler function to deal with the incoming packet
        // you have to write the code in recv_handler of IoT_device
        void recv_handler (PacketTypes &p) override {
            // in this function, you are "not" allowed to use node::id_to_node(id) !!!!!!!!

            // this is a simple example
            // node 0 broadcasts its message to every node and every node relays the packet "only once" and increases its counter
            // the variable hi is used to examine whether the packet has been received by this node before
            // you can remove the variable hi and create your own routing table in class IoT_device
            std::visit(
                overloaded {
                    [&](IoT_ctrl_packet &packet) { // the device receives a packet from the sink
                        if (hi) {
                            return;
                        }
                        packet.set_pre_ID(get_node_ID());
                        packet.set_nex_ID(BROADCAST_ID);
                        packet.set_dst_ID(BROADCAST_ID);
                        packet.increase_payload_counter();
                        hi = true;
                        send_handler(p);
                        // unsigned mat = l3->getMatID();
                    // unsigned act = l3->getActID();
                    // string msg = l3->getMsg(); // get the msg
                    },
                    [&](IoT_data_packet &packet) { // the device receives a packet
                        (void)packet;
                        // cout << "node " << getNodeID() << " send the packet" << '\n';
                    },
                    [&](AGG_ctrl_packet &packet) {
                        (void)packet;
                        // cout << "node id = " << getNodeID() << ", msg = "  << l3->getMsg() << '\n';
                    },
                    [&](DIS_ctrl_packet &packet) {
                        (void)packet;
                        // cout << "node id = " << getNodeID() << ", parent = "  << l3->get_parent() << '\n';
                    },
                    [](std::monostate) {}
                },
                p
            );

        // you should implement the OSPF algorithm in recv_handler
        // getNodeID() returns the id of the current node

        // The current node's neighbors are already stored in the following variable
        // map<unsigned int,bool> node::phy_neighbors
        // however, this variable is private in the class node
        // You have to use node::getPhyNeighbors to get the variable
        // for example, if you want to print all the neighbors of this node
        // const map<unsigned int,bool> &nblist = getPhyNeighbors();
        // cout << "node " << getNodeID() << "'s nblist: ";
        // for (map<unsigned int,bool>::const_iterator it = nblist.begin(); it != nblist.end(); it ++) {
        //     cout << it->first << ", " ;
        // }
        // cout << '\n';

        // you can use p->get_header()->set_src_ID() or get_src_ID()
        //             p->get_header()->set_dst_ID() or get_dst_ID()
        //             p->get_header()->set_pre_ID() or get_pre_ID()
        //             p->get_header()->set_nex_ID() or get_nex_ID() to change or read the packet header

        // In addition, you can get the packet, header, and payload with the correct type
        // in fact, this is downcasting
        // IoT_data_packet * pkt = dynamic_cast<IoT_data_packet*> (p);
        // IoT_data_header * hdr = dynamic_cast<IoT_data_header*> (p->get_header());
        // IoT_data_payload * pld = dynamic_cast<IoT_data_payload*> (p->get_payload());

        // you can also change the IoT_data_payload setting
        // pld->set_msg(string): to set the message transmitted to the destination

        // Besides, you can use packet::generator::generate() to generate a new packet; note that you should fill the header and payload in the packet
        // moreover, you can use "packet *p2 = packet::generator::replicate(p)" to make a clone p2 of packet p
        // note that if the packet is generated or replicated manually, you must delete it by packet::discard() manually before recv_handler finishes

        // "IMPORTANT":
        // You have to "carefully" fill the correct information (e.g., srcID, dstID, ...) in the packet before you send it
        // Note that if you want to transmit a packet to only one next node (i.e., unicast), then you fill the ID of a specific node to "nexID" in the header
        // Otherwise, i.e., you want to broadcasts, then you fill "BROADCAST_ID" to "nexID" in the header
        // after that, you can use send() to transmit the packet
        // usage: send_handler (p);

        // note that packet p will be discarded (deleted) after recv_handler(); you don't need to manually delete it
    }

    // void add_one_hop_neighbor (unsigned int n_id) { one_hop_neighbors[n_id] = true; }
    // unsigned int get_one_hop_neighbor_num () { return one_hop_neighbors.size(); }

        // IoT_device::generator is derived from node::generator to generate a node
};

class mycomp {
    bool reverse;

    public:
        explicit mycomp(bool revparam = false) : reverse(revparam) {}
        bool operator() (const std::unique_ptr<event> &lhs, const std::unique_ptr<event> &rhs) const;
};

class event {
        static std::priority_queue<std::unique_ptr<event>, std::vector<std::unique_ptr<event>>, mycomp> events;
        static inline unsigned int cur_time; // timer
        static inline unsigned int end_time;

        // get the next event
        static std::unique_ptr<event> get_next_event() {
            if(events.empty()) {
                return nullptr;
            }
            // This is safe and the only way to move a unique_ptr out of a priority_queue. The elements are never actually const.
            std::unique_ptr<event> e = std::move(const_cast<std::unique_ptr<event> &>(events.top())); // NOLINT(cppcoreguidelines-pro-type-const-cast)
            events.pop();
            // cout << events.size() << " events remains" << '\n';
            return e;
        }
        static inline std::hash<std::string> event_seq;
        unsigned int trigger_time = 0;

    protected:
        SET(trigger_time)
        static inline std::vector<std::string> derived_class_names;
        explicit event(unsigned int _trigger_time): trigger_time(_trigger_time) {}
        event() = default;
        event(const event &other) = default;
        event(event &&other) = default;
        event &operator=(const event &other) = default;
        event &operator=(event &&other) = default;
        static void add_event (std::unique_ptr<event> &&e) { events.push(std::move(e)); }
    public:
        virtual void trigger()=0;
        virtual ~event() = default;

        virtual unsigned int event_priority() const = 0;
        static unsigned int get_hash_value(const std::string &string_for_hash) {
            size_t priority = event_seq(string_for_hash);
            return static_cast<unsigned int>(priority);
        }

        static void flush_events () { // only for debug
            std::cout << "**flush begin" << '\n';
            while ( ! events.empty() ) {
                std::cout << std::setw(11) << events.top()->trigger_time << ": " << std::setw(11) << events.top()->event_priority() << '\n';
                events.pop();
            }
            std::cout << "**flush end" << '\n';
        }

        GET(trigger_time)

        static void start_simulate( unsigned int _end_time ) { // the function is used to start the simulation
            end_time = _end_time;
            std::unique_ptr<event> e = get_next_event();
            while (e && e->trigger_time <= end_time ) {
                if ( cur_time > e->trigger_time ) {
                    std::cerr << "cur_time = " << cur_time << ", event trigger_time = " << e->trigger_time << '\n';
                    break;
                    
                }
                cur_time = e->trigger_time;

                // cout << "event trigger_time = " << e->trigger_time << '\n';
                e->print(); // for log
                // cout << " event begin" << '\n';
                e->trigger();
                // cout << " event end" << '\n';
                e = event::get_next_event ();
            }
            // cout << "no more event" << '\n';
        }

        static unsigned int get_cur_time() { return cur_time; }
        static void get_cur_time(unsigned int _cur_time) { cur_time = _cur_time; }
        // static unsigned int getEndTime() { return end_time ; }
        // static void getEndTime(unsigned int _end_time) { end_time = _end_time; }

        virtual void print() const = 0; // the function is used to print the event information

        static void print_registered_event_types () {
            std::cout << "registered event types:\n";
            for (const auto &name: derived_class_names) {
                std::cout << name << '\n';
            }
        }
};
std::priority_queue<std::unique_ptr<event>, std::vector<std::unique_ptr<event>>, mycomp> event::events;

bool mycomp::operator() (const std::unique_ptr<event> &lhs, const std::unique_ptr<event> &rhs) const  {
    // cout << lhs->get_trigger_time() << ", " << rhs->get_trigger_time() << '\n';
    // cout << lhs->type() << ", " << rhs->type() << '\n';
    size_t lhs_pri = lhs->event_priority();
    size_t rhs_pri = rhs->event_priority();
    // cout << "lhs hash = " << lhs_pri << '\n';
    // cout << "rhs hash = " << rhs_pri << '\n';

    bool result = lhs->get_trigger_time() == rhs->get_trigger_time() ? lhs_pri > rhs_pri : lhs->get_trigger_time() > rhs->get_trigger_time();
    return result ^ reverse;
}

class recv_event : public event {
    public:
        class recv_data; // forward declaration

    private:
        unsigned int senderID; // the sender
        unsigned int receiverID; // the receiver; the packet will be given to the receiver
        node::PacketTypes pkt; // the packet
        STATIC_CONSTRUCTOR (
            derived_class_names.emplace_back("recv_event");
        )
        // this constructor cannot be directly called by users; only by generator
        // the packet will be given to the receiver
        recv_event(unsigned int _trigger_time, const recv_data &data) : event(_trigger_time), senderID(data.s_id), receiverID(data.r_id), pkt(data._pkt) {}

    public:
        // recv_event will trigger the recv function
        void trigger() override {
            if (!node::id_to_node(receiverID)){
                std::cerr << "recv_event error: no node " << receiverID << "!" << '\n';
                return ;
            }
            node::id_to_node(receiverID)->recv(pkt);
        }

        unsigned int event_priority() const override {
            std::string string_for_hash;
            string_for_hash = std::to_string(get_trigger_time()) +
                std::to_string(senderID) +
                std::to_string (receiverID) +
                std::to_string (std::visit(overloaded {
                    [](auto &&packet) { return packet.get_packet_ID(); },
                    [](std::monostate) -> unsigned { throw std::domain_error("The packet has not been assigned any specific packet type"); }
                }, pkt));
            return get_hash_value(string_for_hash);
        }

        static void generate(unsigned int _trigger_time, const recv_data &data) {
            add_event(std::unique_ptr<recv_event>(new recv_event(_trigger_time, data)));
        }

        // this class is used to initialize the recv_event
        class recv_data{
            public:
                unsigned int s_id = 0;
                unsigned int r_id = 0;
                node::PacketTypes _pkt;
        };

        // the recv_event::print() function is used for log file
        void print () const override {
            std::visit(overloaded {
                [&](auto &&packet) {
                    std::cout << "time "    << std::setw(11) << event::get_cur_time()
                        << "   recID"       << std::setw(11) << receiverID
                        << "   pktID"       << std::setw(11) << packet.get_packet_ID()
                        << "   srcID"       << std::setw(11) << packet.get_header().get_src_ID()
                        << "   dstID"       << std::setw(11) << packet.get_header().get_dst_ID()
                        << "   preID"       << std::setw(11) << packet.get_header().get_pre_ID()
                        << "   nexID"       << std::setw(11) << packet.get_header().get_nex_ID()
                        << "   "            << packet.type()
                        << packet.addition_information();
                },
                [](std::monostate) {}
            }, pkt);
            //  if ( pkt->type() == "IoT_ctrl_packet" ) cout << "   " << ((IoT_ctrl_payload*)pkt->get_payload())->getCounter();
            std::cout << '\n';
            // cout << pkt->type()
            //      << "   time "       << setw(11) << event::getCurTime()
            //      << "   recID "      << setw(11) << receiverID
            //      << "   pktID"       << setw(11) << pkt->get_packet_ID()
            //      << "   srcID "      << setw(11) << pkt->get_header()->get_src_ID()
            //      << "   dstID"       << setw(11) << pkt->get_header()->get_dst_ID()
            //      << "   preID"       << setw(11) << pkt->get_header()->get_pre_ID()
            //      << "   nexID"       << setw(11) << pkt->get_header()->get_nex_ID()
            //      << '\n';
        }
};

class send_event : public event {
    public:
        class send_data; // forward declaration

    private:
        // this constructor cannot be directly called by users; only by generator
        unsigned int senderID; // the sender
        unsigned int receiverID; // the receiver
        node::PacketTypes pkt; // the packet
        STATIC_CONSTRUCTOR (
            derived_class_names.emplace_back("send_event");
        )
        send_event(unsigned int _trigger_time, const send_data &data) : event(_trigger_time), senderID(data.s_id), receiverID(data.r_id), pkt(data._pkt) {}

    public:
        // send_event will trigger the send function
        void trigger() override {
            if (!node::id_to_node(senderID)){
                std::cerr << "send_event error: no node " << senderID << "!" << '\n';
                return ;
            }
            node::id_to_node(senderID)->send(pkt);
        }

        unsigned int event_priority() const override {
            std::string string_for_hash;
            string_for_hash = std::to_string(get_trigger_time()) +
                std::to_string(senderID) +
                std::to_string (receiverID) +
                std::to_string (std::visit(overloaded {
                    [](auto &&packet) { return packet.get_packet_ID(); },
                    [](std::monostate) -> unsigned { throw std::domain_error("The packet has not been assigned any specific packet type"); }
                }, pkt));
            return get_hash_value(string_for_hash);
        }

        static void generate(unsigned int _trigger_time, const send_data &data) {
            add_event(std::unique_ptr<send_event>(new send_event(_trigger_time, data)));
        }

        // this class is used to initialize the send_event
        class send_data{
            public:
                unsigned int s_id = 0;
                unsigned int r_id = 0;
                node::PacketTypes _pkt;
                unsigned int t = 0;
        };

        void print () const override { // the send_event::print() function is used for log file
            std::visit(overloaded {
                [&](auto &&packet) {
                    std::cout << "time "     << std::setw(11) << event::get_cur_time()
                        << "   senID"       << std::setw(11) << senderID
                        << "   pktID"       << std::setw(11) << packet.get_packet_ID()
                        << "   srcID"       << std::setw(11) << packet.get_header().get_src_ID()
                        << "   dstID"       << std::setw(11) << packet.get_header().get_dst_ID()
                        << "   preID"       << std::setw(11) << packet.get_header().get_pre_ID()
                        << "   nexID"       << std::setw(11) << packet.get_header().get_nex_ID()
                        << "   "            << packet.type()
                        << packet.addition_information()
                        // << "   msg"         << setw(11) << dynamic_cast<IoT_data_payload*>(pkt->get_payload())->getMsg()
                        << '\n';
                },
                [](std::monostate) {}
            }, pkt);
            
            // cout << pkt->type()
            //      << "   time "       << setw(11) << event::getCurTime()
            //      << "   senID "      << setw(11) << senderID
            //      << "   pktID"       << setw(11) << pkt->get_packet_ID()
            //      << "   srcID "      << setw(11) << pkt->get_header()->get_src_ID()
            //      << "   dstID"       << setw(11) << pkt->get_header()->get_dst_ID()
            //      << "   preID"       << setw(11) << pkt->get_header()->get_pre_ID()
            //      << "   nexID"       << setw(11) << pkt->get_header()->get_nex_ID()
            //      << '\n';
        }
};

////////////////////////////////////////////////////////////////////////////////

class IoT_data_pkt_gen_event : public event {
    public:
        class gen_data; // forward declaration

    // this class is used to initialize the IoT_data_pkt_gen_event
        class pkt_gen_data {
            public:
                unsigned int src_id = 0;
                unsigned int dst_id = 0;
                std::string msg;
                // packet *_pkt;
        };

    private:
        // this constructor cannot be directly called by users; only by generator
        unsigned int src; // the src
        unsigned int dst; // the dst
        // packet *pkt; // the packet
        std::string msg;
        STATIC_CONSTRUCTOR (
            derived_class_names.emplace_back("IoT_data_pkt_gen_event");
        )
        IoT_data_pkt_gen_event(unsigned int _trigger_time, const pkt_gen_data &data) : event(_trigger_time), src(data.src_id), dst(data.dst_id), msg(data.msg) {}

    public:
        static void generate(unsigned int _trigger_time, const pkt_gen_data &data) {
            add_event(std::unique_ptr<IoT_data_pkt_gen_event>(new IoT_data_pkt_gen_event(_trigger_time, data)));
        }

        // IoT_data_pkt_gen_event will trigger the packet gen function
        void trigger() override {
            if (!node::id_to_node(src)){
                std::cerr << "IoT_data_pkt_gen_event error: no node " << src << "!" << '\n';
                return ;
            }
            if ( dst != BROADCAST_ID && !node::id_to_node(dst)) {
                std::cerr << "IoT_data_pkt_gen_event error: no node " << dst << "!" << '\n';
                return;
            }

            IoT_data_packet pkt;

            pkt.set_src_ID(src);
            pkt.set_dst_ID(dst);
            pkt.set_pre_ID(src); // this column is not important when the packet is first received by the src (i.e., just generated)
            pkt.set_nex_ID(src); // this column is not important when the packet is first received by the src (i.e., just generated)

            pkt.set_msg(msg);

            recv_event::recv_data e_data;
            e_data.s_id = src;
            e_data.r_id = src; // to make the packet start from the src
            e_data._pkt = pkt;

            recv_event::generate(get_trigger_time(), e_data);
        }

        unsigned int event_priority() const override {
            std::string string_for_hash;
            string_for_hash = std::to_string(get_trigger_time()) + std::to_string(src) + std::to_string (dst) ; //to_string (pkt->get_packet_ID());
            return get_hash_value(string_for_hash);
        }

        // the IoT_data_pkt_gen_event::print() function is used for log file
        void print () const override {
            std::cout << "time "     << std::setw(11) << event::get_cur_time()
                << "        "       << std::setw(11) << " "
                << "        "       << std::setw(11) << " "
                << "   srcID"       << std::setw(11) << src
                << "   dstID"       << std::setw(11) << dst
                << "        "       << std::setw(11) << " "
                << "        "       << std::setw(11) << " "
                << "   IoT_data_packet generating"
                << '\n';
        }
};

class IoT_ctrl_pkt_gen_event : public event {
    public:
        class gen_data; // forward declaration

    // this class is used to initialize the IoT_ctrl_pkt_gen_event
        class pkt_gen_data {
            public:
                unsigned int src_id = 0; // the sink
                unsigned int dst_id = 0; // the node that should update its rule
                // unsigned int mat_id; // the target of the rule
                // unsigned int act_id; // the next hop toward the target recorded in the rule
                std::string msg;
                // double per; // the percentage
                // packet *_pkt;
    };

    private:
        // this constructor cannot be directly called by users; only by generator
        unsigned int src; // the src
        unsigned int dst; // the dst
        // unsigned int mat;
        // unsigned int act;
        // packet *pkt; // the packet
        std::string msg;
        // double per; // percentage
        STATIC_CONSTRUCTOR (
            derived_class_names.emplace_back("IoT_ctrl_pkt_gen_event");
        )
        IoT_ctrl_pkt_gen_event(unsigned int _trigger_time, const pkt_gen_data &data) : event(_trigger_time), src(data.src_id), dst(data.dst_id), msg(data.msg) {}

    public:
        static void generate(unsigned int _trigger_time, const pkt_gen_data &data) {
            add_event(std::unique_ptr<IoT_ctrl_pkt_gen_event>(new IoT_ctrl_pkt_gen_event(_trigger_time, data)));
        }

        // IoT_ctrl_pkt_gen_event will trigger the packet gen function
        void trigger() override {
            IoT_ctrl_packet pkt;

            pkt.set_src_ID(src);
            pkt.set_dst_ID(dst);
            pkt.set_pre_ID(src);
            pkt.set_nex_ID(src);

            // payload
            pkt.set_msg(msg);
            // pld->setMatID(mat);
            // pld->setActID(act);
            // pld->setPer(per);

            recv_event::recv_data e_data;
            e_data.s_id = src;
            e_data.r_id = src;
            e_data._pkt = pkt;

            recv_event::generate(get_trigger_time(), e_data);
        }

        unsigned int event_priority() const override {
            std::string string_for_hash;
            // string_for_hash = to_string(get_trigger_time()) + to_string(src) + to_string(dst) + to_string(mat) + to_string(act); //to_string (pkt->get_packet_ID());
            string_for_hash = std::to_string(get_trigger_time()) + std::to_string(src) + std::to_string(dst) ; //to_string (pkt->get_packet_ID());
            return get_hash_value(string_for_hash);
        }

        // the IoT_ctrl_pkt_gen_event::print() function is used for log file
        void print () const override {
            std::cout << "time "     << std::setw(11) << event::get_cur_time()
                << "        "       << std::setw(11) << " "
                << "        "       << std::setw(11) << " "
                << "   srcID"       << std::setw(11) << src
                << "   dstID"       << std::setw(11) << dst
                << "        "       << std::setw(11) << " "
                << "        "       << std::setw(11) << " "
                //  << "   matID"       << setw(11) << mat
                //  << "   actID"       << setw(11) << act
                << "   IoT_ctrl_packet generating"
                << '\n';
        }
};

class AGG_ctrl_pkt_gen_event : public event {
    public:
        class gen_data; // forward declaration

    // this class is used to initialize the AGG_ctrl_pkt_gen_event
        class pkt_gen_data {
            public:
                unsigned int src_id = 0; // the sink
                unsigned int dst_id = 0; // the node that should update its rule
                // unsigned int mat_id; // the target of the rule
                // unsigned int act_id; // the next hop toward the target recorded in the rule
                std::string msg;
                // double per; // the percentage
                // packet *_pkt;
    };

    private:
        // this constructor cannot be directly called by users; only by generator
        unsigned int src; // the src
        unsigned int dst; // the dst
        // unsigned int mat;
        // unsigned int act;
        // packet *pkt; // the packet
        std::string msg;
        // double per; // percentage
        STATIC_CONSTRUCTOR (
            derived_class_names.emplace_back("AGG_ctrl_pkt_gen_event");
        )
        AGG_ctrl_pkt_gen_event(unsigned int _trigger_time, const pkt_gen_data &data) : event(_trigger_time), src(data.src_id), dst(data.dst_id), msg(data.msg) {}

    public:
        static void generate(unsigned int _trigger_time, const pkt_gen_data &data) {
            add_event(std::unique_ptr<AGG_ctrl_pkt_gen_event>(new AGG_ctrl_pkt_gen_event(_trigger_time, data)));
        }

        // AGG_ctrl_pkt_gen_event will trigger the packet gen function
        void trigger() override {
            AGG_ctrl_packet pkt;
            pkt.set_src_ID(src);
            pkt.set_dst_ID(dst);
            pkt.set_pre_ID(src);
            pkt.set_nex_ID(src);

            // payload
            pkt.set_msg(msg);
            // pld->setMatID(mat);
            // pld->setActID(act);
            // pld->setPer(per);

            recv_event::recv_data e_data;
            e_data.s_id = src;
            e_data.r_id = src;
            e_data._pkt = pkt;

            recv_event::generate(get_trigger_time(), e_data);
        }

        unsigned int event_priority() const override {
            std::string string_for_hash;
            // string_for_hash = to_string(get_trigger_time()) + to_string(src) + to_string(dst) + to_string(mat) + to_string(act); //to_string (pkt->get_packet_ID());
            string_for_hash = std::to_string(get_trigger_time()) + std::to_string(src) + std::to_string(dst) ; //to_string (pkt->get_packet_ID());
            return get_hash_value(string_for_hash);
        }

        // the AGG_ctrl_pkt_gen_event::print() function is used for log file
        void print () const override {
            std::cout << "time "     << std::setw(11) << event::get_cur_time()
                << "        "       << std::setw(11) << " "
                << "        "       << std::setw(11) << " "
                << "   srcID"       << std::setw(11) << src
                << "   dstID"       << std::setw(11) << dst
                << "        "       << std::setw(11) << " "
                << "        "       << std::setw(11) << " "
                //  << "   matID"       << setw(11) << mat
                //  << "   actID"       << setw(11) << act
                << "   AGG_ctrl_packet generating"
                << '\n';
        }
};

class DIS_ctrl_pkt_gen_event : public event {
    public:
        class gen_data; // forward declaration

    // this class is used to initialize the DIS_ctrl_pkt_gen_event
        class pkt_gen_data {
            public:
                unsigned int src_id = 0; // the sink
                unsigned int dst_id = 0; // the node that should update its rule
                // unsigned int mat_id; // the target of the rule
                // unsigned int act_id; // the next hop toward the target recorded in the rule
                unsigned int parent = 0;
                std::string msg;
                // double per; // the percentage
                // packet *_pkt;
    };

    private:
        // this constructor cannot be directly called by users; only by generator
        unsigned int src; // the src
        unsigned int dst; // the dst
        // unsigned int mat;
        // unsigned int act;
        // packet *pkt; // the packet
        std::string msg;
        // double per; // percentage
        unsigned int parent;
        STATIC_CONSTRUCTOR (
            derived_class_names.emplace_back("DIS_ctrl_pkt_gen_event");
        )
        DIS_ctrl_pkt_gen_event(unsigned int _trigger_time, const pkt_gen_data &data) : event(_trigger_time), src(data.src_id), dst(data.dst_id), msg(data.msg), parent(data.parent) {}

    public:
        static void generate(unsigned int _trigger_time, const pkt_gen_data &data) {
            add_event(std::unique_ptr<DIS_ctrl_pkt_gen_event>(new DIS_ctrl_pkt_gen_event(_trigger_time, data)));
        }

        // DIS_ctrl_pkt_gen_event will trigger the packet gen function
        void trigger() override {
            DIS_ctrl_packet pkt;

            pkt.set_src_ID(src);
            pkt.set_dst_ID(dst);
            pkt.set_pre_ID(src);
            pkt.set_nex_ID(src);

            // payload
            pkt.set_parent(parent);
            pkt.set_msg(msg);
            // pld->setMatID(mat);
            // pld->setActID(act);
            // pld->setPer(per);

            recv_event::recv_data e_data;
            e_data.s_id = src;
            e_data.r_id = src;
            e_data._pkt = pkt;

            recv_event::generate(get_trigger_time(), e_data);
        }

        unsigned int event_priority() const override {
            std::string string_for_hash;
            // string_for_hash = to_string(get_trigger_time()) + to_string(src) + to_string(dst) + to_string(mat) + to_string(act); //to_string (pkt->get_packet_ID());
            string_for_hash = std::to_string(get_trigger_time()) + std::to_string(src) + std::to_string(dst) ; //to_string (pkt->get_packet_ID());
            return get_hash_value(string_for_hash);
        }

        // the DIS_ctrl_pkt_gen_event::print() function is used for log file
        void print () const override {
            std::cout << "time "     << std::setw(11) << event::get_cur_time()
                << "        "       << std::setw(11) << " "
                << "        "       << std::setw(11) << " "
                << "   srcID"       << std::setw(11) << src
                << "   dstID"       << std::setw(11) << dst
                << "        "       << std::setw(11) << " "
                << "        "       << std::setw(11) << " "
                //  << "   matID"       << setw(11) << mat
                //  << "   actID"       << setw(11) << act
                << "   DIS_ctrl_packet generating"
                << '\n';
        }
};

////////////////////////////////////////////////////////////////////////////////

class link {
        // all links created in the program
        static inline std::map<std::pair<unsigned int, unsigned int>, std::shared_ptr<link>> id_id_link_table;
        unsigned int id1; // from
        unsigned int id2; // to

    protected:
        link(unsigned int _id1, unsigned int _id2): id1(_id1), id2(_id2) {
            if(id_id_link_table.find({_id1, _id2}) != id_id_link_table.end()){
                throw std::invalid_argument("Duplicate link id");
            }
            if ( BROADCAST_ID == _id1 || BROADCAST_ID == _id2 ) {
                throw std::invalid_argument("BROADCAST_ID cannot be used");
            }
        }
        static void register_link(const std::shared_ptr<link> &link) {
            id_id_link_table[{link->id1, link->id2}] = link;
        }
        static inline std::vector<std::string> derived_class_names;

    public:
        /*
        For the same reason as node, the special members are deleted. See node's
        comment above its special members for more details.
        */
        link() = delete;
        link(const link &other) = delete;
        link(link &&other) = delete;
        link &operator=(const link &other) = delete;
        link &operator=(link &&other) = delete;
        virtual ~link() = default;

        static std::shared_ptr<link> id_id_to_link (unsigned int _id1, unsigned int _id2) {
            const auto it = id_id_link_table.find({_id1, _id2});
            return it != id_id_link_table.cend() ? it->second : nullptr;
        }

    virtual double get_latency() = 0; // you must implement your own latency

        static void del_link (unsigned int _id1, unsigned int _id2) {
            const auto it = id_id_link_table.find({_id1, _id2});
            if (it != id_id_link_table.cend()) {
                id_id_link_table.erase(it);
            }
        }

        static auto get_link_num () { return id_id_link_table.size(); }

        static void print () {
            std::cout << "registered link types:\n";
            for (const auto &name: derived_class_names) {
                std::cout << name << '\n';
            }
        }
};

class simple_link: public link {
    private:
        STATIC_CONSTRUCTOR (
            derived_class_names.emplace_back("simple_link");
        )
        simple_link(unsigned int _id1, unsigned int _id2): link (_id1,_id2){} // this constructor cannot be directly called by users

    public:
        static std::shared_ptr<simple_link> generate(unsigned int _id1, unsigned int _id2) {
            try {
                std::shared_ptr<simple_link> link(new simple_link(_id1, _id2));
                register_link(link);
                return link;
            }
            catch (...) {
                throw;
            }
        }
        double get_latency() override { return ONE_HOP_DELAY; } // you can implement your own latency
};

void node::add_phy_neighbor (unsigned int _id){
    if (id == _id) {
        std::cerr << "Failed to add phy_neighbor: the two nodes are the same" << '\n';
        return;
    }
    if (!id_node_table.contains(_id)) {
        std::cerr << "Failed to add phy_neighbor: node " << _id << " does not exist" << '\n';
        return;
    }
    if (phy_neighbors.contains(_id)) {
        std::cerr << "Failed to add phy_neighbor: node " << _id << " has been added" << '\n';
        return;
    }
    phy_neighbors.insert(_id);

    simple_link::generate(id, _id);
}

// the IoT_data_packet_event function is used to add an initial event
void IoT_data_packet_event(unsigned int src, unsigned int dst = 0, unsigned int t = 0, const std::string &msg = "default") {
    if (!node::id_to_node(src)) {
        std::cerr << "src " << src << ": no such node registered\n";
        return;
    }
    if (dst != BROADCAST_ID && !node::id_to_node(dst)) {
        std::cerr << "dst " << dst << ": no such node registered\n";
        return;
    }

    IoT_data_pkt_gen_event::pkt_gen_data e_data;
    e_data.src_id = src;
    e_data.dst_id = dst;
    e_data.msg = msg;

    // recv_event *e = dynamic_cast<recv_event*> ( event::generator::generate("recv_event",t, (void *)&e_data) );
    IoT_data_pkt_gen_event::generate(t, e_data);
}

// the IoT_ctrl_packet_event function is used to add an initial event

void IoT_ctrl_packet_event(unsigned int src = 0, unsigned int t = event::get_cur_time(), const std::string &msg = "default") {
    // 1st parameter: the source; the destination that want to broadcast a msg with counter 0 (i.e., match ID)
    // 2nd parameter: time (optional)
    // 3rd parameter: msg (optional)
    if (!node::id_to_node(src)) {
        std::cerr << "src " << src << ": no such node registered\n";
        return;
    }

    // unsigned int src = con_id;
    IoT_ctrl_pkt_gen_event::pkt_gen_data e_data;
    e_data.src_id = src;
    e_data.dst_id = BROADCAST_ID;
    // e_data.mat_id = mat;
    // e_data.act_id = act;
    e_data.msg = msg;
    // e_data.per = per;

    IoT_ctrl_pkt_gen_event::generate(t, e_data);
}

// the AGG_ctrl_packet_event function is used to add an initial event
void AGG_ctrl_packet_event(unsigned int src, unsigned int dst = 0, unsigned int t = event::get_cur_time(), const std::string &msg = "default") {
    if (!node::id_to_node(src)) {
        std::cerr << "src " << src << ": no such node registered\n";
        return;
    }
    if (dst != BROADCAST_ID && !node::id_to_node(dst)) {
        std::cerr << "dst " << dst << ": no such node registered\n";
        return;
    }

    AGG_ctrl_pkt_gen_event::pkt_gen_data e_data;
    e_data.src_id = src;
    e_data.dst_id = dst;
    e_data.msg = msg;

    // recv_event *e = dynamic_cast<recv_event*> ( event::generator::generate("recv_event",t, (void *)&e_data) );
    AGG_ctrl_pkt_gen_event::generate(t, e_data);
}

// the DIS_ctrl_packet_event function is used to add an initial event
void DIS_ctrl_packet_event(unsigned int sink_id = 0, unsigned int t = event::get_cur_time(), const std::string &msg = "default") {
    if (!node::id_to_node(sink_id)) {
        std::cerr << "sink_id or id is incorrect" << '\n';
        return;
    }

    DIS_ctrl_pkt_gen_event::pkt_gen_data e_data;
    e_data.src_id = sink_id;
    e_data.dst_id = sink_id;
    e_data.parent = sink_id;
    e_data.msg = msg;

    // recv_event *e = dynamic_cast<recv_event*> ( event::generator::generate("recv_event",t, (void *)&e_data) );
    DIS_ctrl_pkt_gen_event::generate(t, e_data);
}

// send_handler function is used to transmit packet p based on the information in the header
// Note that the packet p will not be discard after send_handler ()

void node::send_handler(PacketTypes &p){
    PacketTypes _p = p; // Copying is replicating
    send_event::send_data e_data;
    e_data.s_id = std::visit(overloaded {
        [](auto &&packet){ return packet.get_header().get_pre_ID(); },
        [](std::monostate) -> unsigned { throw std::domain_error("The packet has not been assigned any specific packet type"); }
    }, _p);
    e_data.r_id = std::visit(overloaded {
        [](auto &&packet){ return packet.get_header().get_nex_ID(); },
        [](std::monostate) -> unsigned { throw std::domain_error("The packet has not been assigned any specific packet type"); }
    }, _p);
    e_data._pkt = _p;
    send_event::generate(event::get_cur_time(), e_data);
}

void node::send(PacketTypes &p){ // this function is called by event; not for the user
    unsigned int _nexID = std::visit(overloaded {
        [](auto &&packet){ return packet.get_header().get_nex_ID(); },
        [](std::monostate) -> unsigned { throw std::domain_error("The packet has not been assigned any specific packet type"); }
    }, p);
    for (const auto &nb_id: phy_neighbors) { // neighbor id
        if (nb_id != _nexID && BROADCAST_ID != _nexID) {continue;} // this neighbor will not receive the packet

        unsigned int trigger_time = event::get_cur_time() + static_cast<unsigned int>(link::id_id_to_link(id, nb_id)->get_latency()); // we simply assume that the delay is fixed
        // cout << "node " << id << " send to node " <<  nb_id << '\n';
        recv_event::recv_data e_data;
        e_data.s_id = id;    // set the sender   (i.e., preID)
        e_data.r_id = nb_id; // set the receiver (i.e., nexID)

        e_data._pkt = p;

        recv_event::generate(trigger_time, e_data);
    }
}

int main() {
    // header::generator::print(); // print all registered headers
    // payload::generator::print(); // print all registered payloads
    // packet::generator::print(); // print all registered packets
    // node::generator::print(); // print all registered nodes
    // event::generator::print(); // print all registered events
    // link::generator::print(); // print all registered links

    // read the input and generate devices
    for (unsigned int id = 0; id < 5; id ++){
        IoT_device::generate(id);
    }

    // please generate the sink by yourself

    // set devices' neighbors
    node::id_to_node(0)->add_phy_neighbor(1);
    node::id_to_node(1)->add_phy_neighbor(0);
    node::id_to_node(0)->add_phy_neighbor(2);
    node::id_to_node(2)->add_phy_neighbor(0);
    node::id_to_node(1)->add_phy_neighbor(2);
    node::id_to_node(2)->add_phy_neighbor(1);
    node::id_to_node(1)->add_phy_neighbor(3);
    node::id_to_node(3)->add_phy_neighbor(1);
    node::id_to_node(2)->add_phy_neighbor(4);
    node::id_to_node(4)->add_phy_neighbor(2);

    // node 0 broadcasts a msg with counter 0 at time 100
    IoT_ctrl_packet_event(0, 100);
    // 1st parameter: the source; the destination that want to broadcast a msg with counter 0 (i.e., match ID)
    // 2nd parameter: time (optional)
    // 3rd parameter: msg for debug information (optional)

    // node 4 sends a packet to node 0 at time 200 --> you need to implement routing tables for IoT_devices
    IoT_data_packet_event(4, 0, 200); // IoT_data_packet
    // 1st parameter: the source node
    // 2nd parameter: the destination node (sink)
    // 3rd parameter: time (optional)
    // 4th parameter: msg for debug (optional)

    AGG_ctrl_packet_event(4, 0, 250);
    // 1st parameter: the source node
    // 2nd parameter: the destination node (sink)
    // 3rd parameter: time (optional)
    // 4th parameter: msg (for storing nb list)

    DIS_ctrl_packet_event(0, 260);
    // 1st parameter: the source node (sink)
    // 2nd parameter: the destination node
    // 3rd parameter: parent
    // 4th parameter: time (optional)
    // 5th parameter: msg for debug (optional)

    // start simulation!!
    event::start_simulate(300);
    // event::flush_events() ;
    // cout << packet::get_live_packet_num() << '\n';
    return 0;
}
