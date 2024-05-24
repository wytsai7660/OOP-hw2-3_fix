#include <climits>
#include <functional>
#include <iomanip>
#include <iostream>
#include <map>
#include <queue>
#include <string>
#include <utility>
#include <vector>

#define SET(var_name) \
    template <typename T> \
    void set_##var_name(const T &_##var_name) { \
        static_assert(std::is_convertible_v<decltype(var_name), T>); \
        (var_name) = static_cast<decltype(var_name)>(_##var_name); \
    }
#define SET_WITH_NAME(setter_name, var_name) \
    template <typename T> \
    void setter_name(const T &_##var_name) { \
        static_assert(std::is_convertible_v<decltype(var_name), T>); \
        (var_name) = static_cast<decltype(var_name)>(_##var_name); \
    }
#define GET(var_name) const auto &get_##var_name() const { return var_name; }
#define GET_WITH_NAME(getter_name, var_name) const auto &getter_name() const { return var_name; }
#define STATIC_CONSTRUCTOR(body) \
    class static_constructor { \
        public: \
            static_constructor() { \
                body \
            } \
    }; \
    static inline static_constructor static_constructor;

class header;
class payload;
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
        // factory concept: generate a header
        class generator {
                // lock the copy constructor
                generator(generator &) = default;
                // store all possible types of header
                static inline std::map<std::string,generator*> prototypes;
            protected:
                // allow derived class to use it
                generator() = default;
                // after you create a new header type, please register the factory of this header type by this function
                void register_header_type(generator *h) { prototypes[h->type()] = h; }
                // you have to implement your own generate() to generate your header
                virtual header* generate() = 0 ;
            public:
                // you have to implement your own type() to return your header type
        	    virtual std::string type() = 0 ;
        	    // this function is used to generate any type of header derived
        	    static header * generate (const std::string &type) {
            		if(prototypes.find(type) != prototypes.end()){ // if this type derived exists
            			return prototypes[type]->generate(); // generate it!!
            		}
            		std::cerr << "no such header type" << '\n'; // otherwise
            		return nullptr;
            	}
            	virtual ~generator() = default;
        };

    protected:
        header():src_ID(BROADCAST_ID),dst_ID(BROADCAST_ID),pre_ID(BROADCAST_ID),nex_ID(BROADCAST_ID){} // this constructor cannot be directly called by users
        static inline std::vector<std::string> derived_class_names;

    private:
        unsigned int src_ID;
        unsigned int dst_ID;
        unsigned int pre_ID;
        unsigned int nex_ID;
        header(header&){} // this constructor cannot be directly called by users
};

class IoT_data_header : public header{
        IoT_data_header(IoT_data_header&){} // cannot be called by users
        STATIC_CONSTRUCTOR (
            derived_class_names.emplace_back("IoT_data_header");
        )

    protected:
        IoT_data_header() = default; // this constructor cannot be directly called by users

    public:
        ~IoT_data_header() override = default;
        std::string type() override { return "IoT_data_header"; }

        // IoT_data_header::generator is derived from header::generator to generate a header
        class generator : public header::generator{
                static generator sample;
                // this constructor is only for sample to register this header type
                generator() { /*cout << "IoT_data_header registered" << '\n';*/ register_header_type(&sample); }
            protected:
                header * generate() override {
                    // cout << "IoT_data_header generated" << '\n';
                    return new IoT_data_header;
                }
            public:
                std::string type() override { return "IoT_data_header";}
                ~generator() override = default;

        };
};
IoT_data_header::generator IoT_data_header::generator::sample;

class IoT_ctrl_header : public header{
        IoT_ctrl_header(IoT_ctrl_header&){} // cannot be called by users
        STATIC_CONSTRUCTOR (
            derived_class_names.emplace_back("IoT_ctrl_header");
        )

    protected:
        IoT_ctrl_header() = default; // this constructor cannot be directly called by users

    public:
        ~IoT_ctrl_header() override = default;
        std::string type() override { return "IoT_ctrl_header"; }

        // IoT_ctrl_header::generator is derived from header::generator to generate a header
        class generator : public header::generator{
                static generator sample;
                // this constructor is only for sample to register this header type
                generator() { /*cout << "IoT_ctrl_header registered" << '\n';*/ register_header_type(&sample); }
            protected:
                header * generate() override {
                    // cout << "IoT_ctrl_header generated" << '\n';
                    return new IoT_ctrl_header;
                }
            public:
                std::string type() override { return "IoT_ctrl_header";}
                ~generator() override = default;

        };
};
IoT_ctrl_header::generator IoT_ctrl_header::generator::sample;

class AGG_ctrl_header : public header{
        AGG_ctrl_header(AGG_ctrl_header&){} // cannot be called by users
        STATIC_CONSTRUCTOR (
            derived_class_names.emplace_back("AGG_ctrl_header");
        )

    protected:
        AGG_ctrl_header() = default; // this constructor cannot be directly called by users

    public:
        ~AGG_ctrl_header() override = default;
        std::string type() override { return "AGG_ctrl_header"; }

        // AGG_ctrl_header::generator is derived from header::generator to generate a header
        class generator : public header::generator{
                static generator sample;
                // this constructor is only for sample to register this header type
                generator() { /*cout << "AGG_ctrl_header registered" << '\n';*/ register_header_type(&sample); }
            protected:
                header * generate() override {
                    // cout << "AGG_ctrl_header generated" << '\n';
                    return new AGG_ctrl_header;
                }
            public:
                std::string type() override { return "AGG_ctrl_header";}
                ~generator() override = default;

        };
};
AGG_ctrl_header::generator AGG_ctrl_header::generator::sample;

class DIS_ctrl_header : public header{
        DIS_ctrl_header(DIS_ctrl_header&){} // cannot be called by users
        STATIC_CONSTRUCTOR (
            derived_class_names.emplace_back("DIS_ctrl_header");
        )

    protected:
        DIS_ctrl_header() = default; // this constructor cannot be directly called by users

    public:
        ~DIS_ctrl_header() override = default;
        std::string type() override { return "DIS_ctrl_header"; }

        // DIS_ctrl_header::generator is derived from header::generator to generate a header
        class generator : public header::generator{
                static generator sample;
                // this constructor is only for sample to register this header type
                generator() { /*cout << "DIS_ctrl_header registered" << '\n';*/ register_header_type(&sample); }
            protected:
                header * generate() override {
                    // cout << "DIS_ctrl_header generated" << '\n';
                    return new DIS_ctrl_header;
                }
            public:
                std::string type() override { return "DIS_ctrl_header";}
                ~generator() override = default;

        };
};
DIS_ctrl_header::generator DIS_ctrl_header::generator::sample;

class payload {
        payload(payload&){} // this constructor cannot be directly called by users

        std::string msg;

    protected:
        payload() = default;
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

        class generator {
                // lock the copy constructor
                generator(generator &) = default;
                // store all possible types of header
                static inline std::map<std::string,generator*> prototypes;
            protected:
                // allow derived class to use it
                generator() = default;
                // after you create a new payload type, please register the factory of this payload type by this function
                void register_payload_type(generator *h) { prototypes[h->type()] = h; }
                // you have to implement your own generate() to generate your payload
                virtual payload* generate() = 0;
            public:
                // you have to implement your own type() to return your header type
        	    virtual std::string type() = 0;
        	    // this function is used to generate any type of header derived
        	    static payload * generate (const std::string &type) {
            		if(prototypes.find(type) != prototypes.end()){ // if this type derived exists
            			return prototypes[type]->generate(); // generate it!!
            		}
            		std::cerr << "no such payload type" << '\n'; // otherwise
            		return nullptr;
            	}
            	virtual ~generator() = default;
        };
};

class IoT_data_payload : public payload {
        IoT_data_payload(IoT_data_payload&){}
        STATIC_CONSTRUCTOR (
            derived_class_names.emplace_back("IoT_data_payload");
        )

    protected:
        IoT_data_payload() = default; // this constructor cannot be directly called by users
    public:
        ~IoT_data_payload() override = default;

        std::string type() override { return "IoT_data_payload"; }

        // IoT_data_payload::generator is derived from payload::generator to generate a payload
        class generator : public payload::generator{
                static generator sample;
                // this constructor is only for sample to register this payload type
                generator() { /*cout << "IoT_data_payload registered" << '\n';*/ register_payload_type(&sample); }
            protected:
                payload * generate() override {
                    // cout << "IoT_data_payload generated" << '\n';
                    return new IoT_data_payload;
                }
            public:
                std::string type() override { return "IoT_data_payload";}
                ~generator() override = default;
        };
};
IoT_data_payload::generator IoT_data_payload::generator::sample;

class IoT_ctrl_payload : public payload {
        IoT_ctrl_payload(IoT_ctrl_payload & s): counter (s.counter) {}
        STATIC_CONSTRUCTOR (
            derived_class_names.emplace_back("IoT_ctrl_payload");
        )

        unsigned int counter ;

    protected:
        IoT_ctrl_payload(): counter (0) {} // this constructor cannot be directly called by users
    public:
        ~IoT_ctrl_payload() override = default;

        void increase() { counter ++; } // used to increase the counter
        GET(counter) // used to get the value of counter

        std::string type() override { return "IoT_ctrl_payload"; }

        // IoT_ctrl_payload::generator is derived from payload::generator to generate a payload
        class generator : public payload::generator{
                static generator sample;
                // this constructor is only for sample to register this payload type
                generator() { /*cout << "IoT_ctrl_payload registered" << '\n';*/ register_payload_type(&sample); }
            protected:
                payload * generate() override {
                    // cout << "IoT_ctrl_payload generated" << '\n';
                    return new IoT_ctrl_payload;
                }
            public:
                std::string type() override { return "IoT_ctrl_payload";}
                ~generator() override = default;
        };
};
IoT_ctrl_payload::generator IoT_ctrl_payload::generator::sample;

class AGG_ctrl_payload : public payload {
        AGG_ctrl_payload(AGG_ctrl_payload & s) {}
        STATIC_CONSTRUCTOR (
            derived_class_names.emplace_back("AGG_ctrl_payload");
        )

        // unsigned int counter ;

    protected:
        AGG_ctrl_payload() = default; // this constructor cannot be directly called by users
    public:
        ~AGG_ctrl_payload() override = default;

        // void increase() { counter ++; } // used to increase the counter
        // GET(getCounter,unsigned int,counter); // used to get the value of counter

        std::string type() override { return "AGG_ctrl_payload"; }

        // AGG_ctrl_payload::generator is derived from payload::generator to generate a payload
        class generator : public payload::generator{
                static generator sample;
                // this constructor is only for sample to register this payload type
                generator() { /*cout << "AGG_ctrl_payload registered" << '\n';*/ register_payload_type(&sample); }
            protected:
                payload * generate() override {
                    // cout << "AGG_ctrl_payload generated" << '\n';
                    return new AGG_ctrl_payload;
                }
            public:
                std::string type() override { return "AGG_ctrl_payload";}
                ~generator() override = default;
        };
};
AGG_ctrl_payload::generator AGG_ctrl_payload::generator::sample;

class DIS_ctrl_payload : public payload {
        DIS_ctrl_payload(DIS_ctrl_payload & s): parent(s.parent) {}
        STATIC_CONSTRUCTOR (
            derived_class_names.emplace_back("DIS_ctrl_payload");
        )

        // unsigned int counter ;
        unsigned int parent;

    protected:
        explicit DIS_ctrl_payload(int _parent=0): parent (_parent) {} // this constructor cannot be directly called by users
    public:
        ~DIS_ctrl_payload() override = default;

        // void increase() { counter ++; } // used to increase the counter
        SET(parent)
        GET(parent) // used to get the value of counter

        std::string type() override { return "DIS_ctrl_payload"; }

        // DIS_ctrl_payload::generator is derived from payload::generator to generate a payload
        class generator : public payload::generator{
                static generator sample;
                // this constructor is only for sample to register this payload type
                generator() { /*cout << "DIS_ctrl_payload registered" << '\n';*/ register_payload_type(&sample); }
            protected:
                payload * generate() override {
                    // cout << "DIS_ctrl_payload generated" << '\n';
                    return new DIS_ctrl_payload;
                }
            public:
                std::string type() override { return "DIS_ctrl_payload";}
                ~generator() override = default;
        };
};
DIS_ctrl_payload::generator DIS_ctrl_payload::generator::sample;

class packet{
        // a packet usually contains a header and a payload
        header *hdr;
        payload *pld;
        unsigned int p_id;
        static inline unsigned int last_packet_id ;

        packet(packet &) {}
        static inline int live_packet_num ;
    protected:
        // these constructors cannot be directly called by users
        packet(): hdr(nullptr), pld(nullptr) { p_id=last_packet_id++; live_packet_num ++; }
        packet(const std::string &_hdr, const std::string &_pld, bool rep = false, unsigned int rep_id = 0) {
            if (! rep )  { // a duplicated packet does not have a new packet id
                p_id = last_packet_id ++;
            }
            else {
                p_id = rep_id;
            }
            hdr = header::generator::generate(_hdr);
            pld = payload::generator::generate(_pld);
            live_packet_num ++;
        }
        static inline std::vector<std::string> derived_class_names;
    public:
        virtual ~packet(){
            // cout << "packet destructor begin" << '\n';
            if (hdr != nullptr) {
                delete hdr;
            }
            if (pld != nullptr) {
                delete pld;
            }
            live_packet_num --;
            // cout << "packet destructor end" << '\n';
        }

        SET_WITH_NAME(set_header, hdr)
        GET_WITH_NAME(get_header, hdr)
        SET_WITH_NAME(set_payload, pld)
        GET_WITH_NAME(get_payload, pld)
        GET_WITH_NAME(get_packet_ID, p_id)

        static void discard ( packet* &p ) {
            // cout << "checking" << '\n';
            if (p != nullptr) {
                // cout << "discarding" << '\n';
                // cout << p->type() << '\n';
                delete p;
                // cout << "discarded" << '\n';
            }
            p = nullptr;
            // cout << "checked" << '\n';
        }
        virtual std::string type () = 0;
        // you can define your own packet's addition_information
        // to print more information for recv_event and send_event
        virtual std::string addition_information () { return ""; }

        static int get_live_packet_num () { return live_packet_num; }

        static void print () {
            std::cout << "registered packet types:\n";
            for (const auto &name: derived_class_names) {
                std::cout << name << '\n';
            }
        }

        class generator {
                // lock the copy constructor
                generator(generator &) = default;
                // store all possible types of packet
                static inline std::map<std::string,generator*> prototypes;
            protected:
                // allow derived class to use it
                generator() = default;
                // after you create a new packet type, please register the factory of this payload type by this function
                void register_packet_type(generator *h) { prototypes[h->type()] = h; }
                // you have to implement your own generate() to generate your payload
                virtual packet* generate ( packet *p = nullptr) = 0;
            public:
                // you have to implement your own type() to return your packet type
        	    virtual std::string type() = 0;
        	    // this function is used to generate any type of packet derived
        	    static packet * generate (const std::string &type) {
            		if(prototypes.find(type) != prototypes.end()){ // if this type derived exists
            			return prototypes[type]->generate(); // generate it!!
            		}
            		std::cerr << "no such packet type" << '\n'; // otherwise
            		return nullptr;
            	}
            	static packet * replicate (packet *p) {
            	    if(prototypes.find(p->type()) != prototypes.end()){ // if this type derived exists
            			return prototypes[p->type()]->generate(p); // generate it!!
            		}
            		std::cerr << "no such packet type" << '\n'; // otherwise
            		return nullptr;
            	}
            	virtual ~generator() = default;
        };
};

// this packet is used to transmit the data
class IoT_data_packet: public packet {
        IoT_data_packet(IoT_data_packet &) {}
        STATIC_CONSTRUCTOR (
            derived_class_names.emplace_back("IoT_data_packet");
        )

    protected:
        IoT_data_packet() = default; // this constructor cannot be directly called by users
        explicit IoT_data_packet(packet*p): packet(p->get_header()->type(), p->get_payload()->type(), true, p->get_packet_ID()) {
            *(dynamic_cast<IoT_data_header*>(this->get_header())) = *(dynamic_cast<IoT_data_header*> (p->get_header()));
            *(dynamic_cast<IoT_data_payload*>(this->get_payload())) = *(dynamic_cast<IoT_data_payload*> (p->get_payload()));
            //DFS_path = (dynamic_cast<IoT_data_header*>(p))->DFS_path;
            //isVisited = (dynamic_cast<IoT_data_header*>(p))->isVisited;
        } // for duplicate
        IoT_data_packet(const std::string &_h, const std::string &_p): packet(_h, _p) {}

    public:
        ~IoT_data_packet() override = default;
        std::string type() override { return "IoT_data_packet"; }

        // IoT_data_packet::generator is derived from packet::generator to generate a pub packet
        class generator : public packet::generator{
                static generator sample;
                // this constructor is only for sample to register this packet type
                generator() { /*cout << "IoT_data_packet registered" << '\n';*/ register_packet_type(&sample); }
            protected:
                packet *generate (packet *p = nullptr) override {
                    // cout << "IoT_data_packet generated" << '\n';
                    if ( nullptr == p ) {
                        return new IoT_data_packet("IoT_data_header","IoT_data_payload");
                    }
                    return new IoT_data_packet(p); // duplicate
                }
            public:
                std::string type() override { return "IoT_data_packet";}
                ~generator() override = default;
        };
};
IoT_data_packet::generator IoT_data_packet::generator::sample;

// this packet type is used to conduct distributed BFS
class IoT_ctrl_packet: public packet {
        IoT_ctrl_packet(IoT_ctrl_packet &) {}
        STATIC_CONSTRUCTOR (
            derived_class_names.emplace_back("IoT_ctrl_packet");
        )

    protected:
        IoT_ctrl_packet() = default; // this constructor cannot be directly called by users
        explicit IoT_ctrl_packet(packet*p): packet(p->get_header()->type(), p->get_payload()->type(), true, p->get_packet_ID()) {
            *(dynamic_cast<IoT_ctrl_header*>(this->get_header())) = *(dynamic_cast<IoT_ctrl_header*> (p->get_header()));
            *(dynamic_cast<IoT_ctrl_payload*>(this->get_payload())) = *(dynamic_cast<IoT_ctrl_payload*> (p->get_payload()));
            //DFS_path = (dynamic_cast<IoT_ctrl_header*>(p))->DFS_path;
            //isVisited = (dynamic_cast<IoT_ctrl_header*>(p))->isVisited;
        } // for duplicate
        IoT_ctrl_packet(const std::string &_h, const std::string &_p): packet(_h, _p) {}

    public:
        ~IoT_ctrl_packet() override = default;
        std::string type() override { return "IoT_ctrl_packet"; }
        std::string addition_information() override {
            unsigned int counter = (dynamic_cast<IoT_ctrl_payload*>(this->get_payload()))->get_counter();
            // cout << counter << '\n';
            return " counter " + std::to_string(counter);
        }

        // IoT_ctrl_packet::generator is derived from packet::generator to generate a pub packet
        class generator : public packet::generator{
                static generator sample;
                // this constructor is only for sample to register this packet type
                generator() { /*cout << "IoT_ctrl_packet registered" << '\n';*/ register_packet_type(&sample); }
            protected:
                packet *generate (packet *p = nullptr) override {
                    // cout << "IoT_ctrl_packet generated" << '\n';
                    if ( nullptr == p ) {
                        return new IoT_ctrl_packet("IoT_ctrl_header","IoT_ctrl_payload");
                    }
                    return new IoT_ctrl_packet(p); // duplicate
                }
            public:
                std::string type() override { return "IoT_ctrl_packet";}
                ~generator() override = default;
        };
};
IoT_ctrl_packet::generator IoT_ctrl_packet::generator::sample;

// this packet type is used to transmit each device's nblist to the sink
class AGG_ctrl_packet: public packet {
        AGG_ctrl_packet(AGG_ctrl_packet &) {}
        STATIC_CONSTRUCTOR (
            derived_class_names.emplace_back("AGG_ctrl_packet");
        )

    protected:
        AGG_ctrl_packet() = default; // this constructor cannot be directly called by users
        explicit AGG_ctrl_packet(packet*p): packet(p->get_header()->type(), p->get_payload()->type(), true, p->get_packet_ID()) {
            *(dynamic_cast<AGG_ctrl_header*>(this->get_header())) = *(dynamic_cast<AGG_ctrl_header*> (p->get_header()));
            *(dynamic_cast<AGG_ctrl_payload*>(this->get_payload())) = *(dynamic_cast<AGG_ctrl_payload*> (p->get_payload()));
            //DFS_path = (dynamic_cast<AGG_ctrl_header*>(p))->DFS_path;
            //isVisited = (dynamic_cast<AGG_ctrl_header*>(p))->isVisited;
        } // for duplicate
        AGG_ctrl_packet(const std::string &_h, const std::string &_p): packet(_h, _p) {}

    public:
        ~AGG_ctrl_packet() override = default;
        std::string type() override { return "AGG_ctrl_packet"; }

        // virtual string addition_information() {
        //     string msg = (dynamic_cast<AGG_ctrl_payload*>(this->get_payload()))->getMsg();
        //     return " msg " + msg;
        // }

        // AGG_ctrl_packet::generator is derived from packet::generator to generate a pub packet
        class generator : public packet::generator{
                static generator sample;
                // this constructor is only for sample to register this packet type
                generator() { /*cout << "AGG_ctrl_packet registered" << '\n';*/ register_packet_type(&sample); }
            protected:
                packet *generate (packet *p = nullptr) override {
                    // cout << "AGG_ctrl_packet generated" << '\n';
                    if ( nullptr == p ) {
                        return new AGG_ctrl_packet("AGG_ctrl_header","AGG_ctrl_payload");
                    }
                    return new AGG_ctrl_packet(p); // duplicate
                }
            public:
                std::string type() override { return "AGG_ctrl_packet";}
                ~generator() override = default;
        };
};
AGG_ctrl_packet::generator AGG_ctrl_packet::generator::sample;

// this packet type is used to transmit the new parent to each device
class DIS_ctrl_packet: public packet {
        DIS_ctrl_packet(DIS_ctrl_packet &) {}
        STATIC_CONSTRUCTOR (
            derived_class_names.emplace_back("DIS_ctrl_packet");
        )

    protected:
        DIS_ctrl_packet() = default; // this constructor cannot be directly called by users
        explicit DIS_ctrl_packet(packet*p): packet(p->get_header()->type(), p->get_payload()->type(), true, p->get_packet_ID()) {
            *(dynamic_cast<DIS_ctrl_header*>(this->get_header())) = *(dynamic_cast<DIS_ctrl_header*> (p->get_header()));
            *(dynamic_cast<DIS_ctrl_payload*>(this->get_payload())) = *(dynamic_cast<DIS_ctrl_payload*> (p->get_payload()));
            //DFS_path = (dynamic_cast<DIS_ctrl_header*>(p))->DFS_path;
            //isVisited = (dynamic_cast<DIS_ctrl_header*>(p))->isVisited;
        } // for duplicate
        DIS_ctrl_packet(const std::string &_h, const std::string &_p): packet(_h, _p) {}

    public:
        ~DIS_ctrl_packet() override = default;
        std::string type() override { return "DIS_ctrl_packet"; }
        std::string addition_information() override {
            unsigned int parent = (dynamic_cast<DIS_ctrl_payload*>(this->get_payload()))->get_parent();
            // cout << counter << '\n';
            return " parent " + std::to_string(parent);
        }

        // DIS_ctrl_packet::generator is derived from packet::generator to generate a pub packet
        class generator : public packet::generator{
                static generator sample;
                // this constructor is only for sample to register this packet type
                generator() { /*cout << "DIS_ctrl_packet registered" << '\n';*/ register_packet_type(&sample); }
            protected:
                packet *generate (packet *p = nullptr) override {
                    // cout << "DIS_ctrl_packet generated" << '\n';
                    if ( nullptr == p ) {
                        return new DIS_ctrl_packet("DIS_ctrl_header","DIS_ctrl_payload");
                    }
                    return new DIS_ctrl_packet(p); // duplicate
                }
            public:
                std::string type() override { return "DIS_ctrl_packet";}
                ~generator() override = default;
        };
};
DIS_ctrl_packet::generator DIS_ctrl_packet::generator::sample;

class node {
        // all nodes created in the program
        static inline std::map<unsigned int, node*> id_node_table;
        unsigned int id;
        std::map<unsigned int,bool> phy_neighbors;

    protected:
        node(node&){} // this constructor should not be used
        node() = default; // this constructor should not be used
        explicit node(unsigned int _id): id(_id) { id_node_table[_id] = this; }
        static inline std::vector<std::string> derived_class_names;
    public:
        virtual ~node() { // erase the node
            id_node_table.erase (id) ;
        }
        virtual std::string type() = 0; // please define it in your derived node class

        void add_phy_neighbor (unsigned int _id, const std::string &link_type = "simple_link"); // we only add a directed link from id to _id
        void del_phy_neighbor (unsigned int _id) { // we only delete a directed link from id to _id
            phy_neighbors.erase(_id);
        }

        // you can use the function to get the node's neighbors at this time
        // but in the project 3, you are not allowed to use this function
        const std::map<unsigned int,bool> & get_phy_neighbors () {
            return phy_neighbors;
        }


        void recv (packet *p) {
            packet *tp = p;
            recv_handler(tp);
            packet::discard(p);
        } // the packet will be directly deleted after the handler
        void send (packet *p);

        // receive the packet and do something; this is a pure virtual function
        virtual void recv_handler(packet *p) = 0;
        void send_handler(packet *P);

        static node * id_to_node (unsigned int _id) { return ((id_node_table.find(_id)!=id_node_table.end()) ? id_node_table[_id]: nullptr) ; }
        GET_WITH_NAME(get_node_ID, id)

        static void del_node (unsigned int _id) {
            if (id_node_table.find(_id) != id_node_table.end()) {
                id_node_table.erase(_id);
            }
        }
        static unsigned int get_node_num () { return id_node_table.size(); }

        static void print () {
            std::cout << "registered node types:\n";
            for (const auto &name: derived_class_names) {
                std::cout << name << '\n';
            }
        }

        class generator {
                // lock the copy constructor
                generator(generator &) = default;
                // store all possible types of node
                static inline std::map<std::string, generator*> prototypes;
            protected:
                // allow derived class to use it
                generator() = default;
                // after you create a new node type, please register the factory of this node type by this function
                void register_node_type(generator *h) { prototypes[h->type()] = h; }
                // you have to implement your own generate() to generate your node
                virtual node* generate(unsigned int _id) = 0;
            public:
                // you have to implement your own type() to return your node type
        	    virtual std::string type() = 0;
        	    // this function is used to generate any type of node derived
        	    static node * generate (const std::string &type, unsigned int _id) {
        	        if(id_node_table.find(_id)!=id_node_table.end()){
        	            std::cerr << "duplicate node id" << '\n'; // node id is duplicated
        	            return nullptr;
        	        }
        	        if ( BROADCAST_ID == _id ) {
        	            std::cerr << "BROADCAST_ID cannot be used" << '\n';
        	            return nullptr;
        	        }
            		if(prototypes.find(type) != prototypes.end()){ // if this type derived exists
            		    node * created_node = prototypes[type]->generate(_id);
            			return created_node; // generate it!!
            		}
            		std::cerr << "no such node type" << '\n'; // otherwise
            		return nullptr;
            	}
            	virtual ~generator() = default;
        };
};

class IoT_device: public node {
        // map<unsigned int,bool> one_hop_neighbors; // you can use this variable to record the node's 1-hop neighbors
        STATIC_CONSTRUCTOR (
            derived_class_names.emplace_back("IoT_device");
        )

        bool hi; // this is used for example; you can remove it when doing hw2

    protected:
        IoT_device() = default; // it should not be used
        IoT_device(IoT_device&) {} // it should not be used
        explicit IoT_device(unsigned int _id): node(_id), hi(false) {} // this constructor cannot be directly called by users

    public:
        ~IoT_device() override = default;
        std::string type() override { return "IoT_device"; }

        // please define recv_handler function to deal with the incoming packet
        // you have to write the code in recv_handler of IoT_device
        void recv_handler (packet *p) override {
            // in this function, you are "not" allowed to use node::id_to_node(id) !!!!!!!!

            // this is a simple example
            // node 0 broadcasts its message to every node and every node relays the packet "only once" and increases its counter
            // the variable hi is used to examine whether the packet has been received by this node before
            // you can remove the variable hi and create your own routing table in class IoT_device
            if (p == nullptr) {return ;}

            if (p->type() == "IoT_ctrl_packet" && !hi ) { // the device receives a packet from the sink
                IoT_ctrl_packet *p3 = nullptr;
                p3 = dynamic_cast<IoT_ctrl_packet*> (p);
                IoT_ctrl_payload *l3 = nullptr;
                l3 = dynamic_cast<IoT_ctrl_payload*> (p3->get_payload());

                p3->get_header()->set_pre_ID ( get_node_ID() );
                p3->get_header()->set_nex_ID ( BROADCAST_ID );
                p3->get_header()->set_dst_ID ( BROADCAST_ID );

                l3->increase();
                hi = true;
                send_handler(p3);
                // unsigned mat = l3->getMatID();
                // unsigned act = l3->getActID();
                // string msg = l3->getMsg(); // get the msg
            }
            else if (p->type() == "IoT_data_packet" ) { // the device receives a packet
                // cout << "node " << getNodeID() << " send the packet" << '\n';
            }
            else if (p->type() == "AGG_ctrl_packet") {
                AGG_ctrl_packet *p3 = nullptr;
                p3 = dynamic_cast<AGG_ctrl_packet*> (p);
                AGG_ctrl_payload *l3 = nullptr;
                l3 = dynamic_cast<AGG_ctrl_payload*> (p3->get_payload());

                // cout << "node id = " << getNodeID() << ", msg = "  << l3->getMsg() << '\n';
            }
            else if (p->type() == "DIS_ctrl_packet") {
                DIS_ctrl_packet *p3 = nullptr;
                p3 = dynamic_cast<DIS_ctrl_packet*> (p);
                DIS_ctrl_payload *l3 = nullptr;
                l3 = dynamic_cast<DIS_ctrl_payload*> (p3->get_payload());

                // cout << "node id = " << getNodeID() << ", parent = "  << l3->get_parent() << '\n';
            }

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
        class generator : public node::generator{
                static generator sample;
                // this constructor is only for sample to register this node type
                generator() { /*cout << "IoT_device registered" << '\n';*/ register_node_type(&sample); }
            protected:
                node * generate(unsigned int _id) override { /*cout << "IoT_device generated" << '\n';*/ return new IoT_device(_id); }
            public:
                std::string type() override { return "IoT_device";}
                ~generator() override = default;
        };
};
IoT_device::generator IoT_device::generator::sample;

class mycomp {
    bool reverse;

    public:
        explicit mycomp(const bool& revparam = false) { reverse=revparam ; }
        bool operator() (const event* lhs, const event* rhs) const;
};

class event {
        explicit event(event*&){} // this constructor cannot be directly called by users
        static inline std::priority_queue < event*, std::vector < event* >, mycomp > events;
        static inline unsigned int cur_time; // timer
        static inline unsigned int end_time;

        // get the next event
        static event * get_next_event() {
            if(events.empty()) {
                return nullptr;
            }
            event * e = events.top();
            events.pop();
            // cout << events.size() << " events remains" << '\n';
            return e;
        }
        static void add_event (event *e) { events.push(e); }
        static inline std::hash<std::string> event_seq;

    protected:
        unsigned int trigger_time;
        static inline std::vector<std::string> derived_class_names;
        event() = default; // it should not be used
        explicit event(unsigned int _trigger_time): trigger_time(_trigger_time) {}

    public:
        virtual void trigger()=0;
        virtual ~event() = default;

        virtual unsigned int event_priority() const = 0;
        unsigned int get_hash_value(const std::string &string_for_hash) const {
            unsigned int priority = event_seq (string_for_hash);
            return priority;
        }

        static void flush_events () { // only for debug
            std::cout << "**flush begin" << '\n';
            while ( ! events.empty() ) {
                std::cout << std::setw(11) << events.top()->trigger_time << ": " << std::setw(11) << events.top()->event_priority() << '\n';
                delete events.top();
                events.pop();
            }
            std::cout << "**flush end" << '\n';
        }

        GET(trigger_time)

        static void start_simulate( unsigned int _end_time ) { // the function is used to start the simulation
            if (_end_time<0) {
                std::cerr << "you should give a positive value of _end_time" << '\n';
                return;
            }
            end_time = _end_time;
            event *e;
            e = event::get_next_event ();
            while ( e != nullptr && e->trigger_time <= end_time ) {
                if ( cur_time <= e->trigger_time ) {
                    cur_time = e->trigger_time;
                }
                else {
                    std::cerr << "cur_time = " << cur_time << ", event trigger_time = " << e->trigger_time << '\n';
                    break;
                }

                // cout << "event trigger_time = " << e->trigger_time << '\n';
                e->print(); // for log
                // cout << " event begin" << '\n';
                e->trigger();
                // cout << " event end" << '\n';
                delete e;
                e = event::get_next_event ();
            }
            // cout << "no more event" << '\n';
        }

        static unsigned int get_cur_time() { return cur_time ; }
        static void get_cur_time(unsigned int _cur_time) { cur_time = _cur_time; }
        // static unsigned int getEndTime() { return end_time ; }
        // static void getEndTime(unsigned int _end_time) { end_time = _end_time; }

        virtual void print () const = 0; // the function is used to print the event information

        static void print_registered_event_types () {
            std::cout << "registered event types:\n";
            for (const auto &name: derived_class_names) {
                std::cout << name << '\n';
            }
        }

        template <typename DataType>
        class generator{
                // lock the copy constructor
                generator(generator &) = default;
                // store all possible types of event
                static inline std::map<std::string, generator*> prototypes;
            protected:
                // allow derived class to use it
                generator() = default;
                // after you create a new event type, please register the factory of this event type by this function
                void register_event_type(generator *h) {
                    prototypes[h->type()] = h;
                    registered_event_names.push_back(h->type());
                }
                // you have to implement your own generate() to generate your event
                virtual event* generate(unsigned int _trigger_time, const DataType &data) = 0;
            public:
                // you have to implement your own type() to return your event type
        	    virtual std::string type() = 0;
        	    // this function is used to generate any type of event derived
        	    static event * generate (const std::string &type, unsigned int _trigger_time, const DataType &data) {
            		if(prototypes.find(type) != prototypes.end()){ // if this type derived exists
            		    event * e = prototypes[type]->generate(_trigger_time, data);
            		    add_event(e);
            		    return e; // generate it!!
            		}
            		std::cerr << "no such event type" << '\n'; // otherwise
            		return nullptr;
            	}
            	virtual ~generator() = default;
        };
    private:
        static inline std::vector<std::string> registered_event_names;
};

bool mycomp::operator() (const event* lhs, const event* rhs) const  {
    // cout << lhs->get_trigger_time() << ", " << rhs->get_trigger_time() << '\n';
    // cout << lhs->type() << ", " << rhs->type() << '\n';
    unsigned int lhs_pri = lhs->event_priority();
    unsigned int rhs_pri = rhs->event_priority();
    // cout << "lhs hash = " << lhs_pri << '\n';
    // cout << "rhs hash = " << rhs_pri << '\n';

    if (reverse) {
        return ((lhs->get_trigger_time()) == (rhs->get_trigger_time())) ? (lhs_pri < rhs_pri): ((lhs->get_trigger_time()) < (rhs->get_trigger_time()));
    }
    return ((lhs->get_trigger_time()) == (rhs->get_trigger_time())) ? (lhs_pri > rhs_pri): ((lhs->get_trigger_time()) > (rhs->get_trigger_time()));
}

class recv_event: public event {
    public:
        class recv_data; // forward declaration

    private:
        recv_event(recv_event&) {} // this constructor cannot be used
        recv_event() = default; // we don't allow users to new a recv_event by themselves
        unsigned int senderID; // the sender
        unsigned int receiverID; // the receiver; the packet will be given to the receiver
        packet *pkt; // the packet
        STATIC_CONSTRUCTOR (
            derived_class_names.emplace_back("recv_event");
        )

    protected:
        // this constructor cannot be directly called by users; only by generator
        // the packet will be given to the receiver
        recv_event(unsigned int _trigger_time, const recv_data &data): event(_trigger_time), senderID(data.s_id), receiverID(data.r_id), pkt(data._pkt){}

    public:
        ~recv_event() override = default;
        // recv_event will trigger the recv function
        void trigger() override {
            if (pkt == nullptr) {
                std::cerr << "recv_event error: no pkt!" << '\n';
                return ;
            }
            if (node::id_to_node(receiverID) == nullptr){
                std::cerr << "recv_event error: no node " << receiverID << "!" << '\n';
                delete pkt; return ;
            }
            node::id_to_node(receiverID)->recv(pkt);
        }

        unsigned int event_priority() const override
        {
            std::string string_for_hash;
            string_for_hash = std::to_string(get_trigger_time()) + std::to_string(senderID) + std::to_string (receiverID) + std::to_string (pkt->get_packet_ID());
            return get_hash_value(string_for_hash);
        }

        // recv_event::generator is derived from event::generator to generate a event
        class generator : public event::generator<recv_data>{
                static generator sample;
                // this constructor is only for sample to register this event type
                generator() { /*cout << "recv_event registered" << '\n';*/ register_event_type(&sample); }
            protected:
                event * generate(unsigned int _trigger_time, const recv_data &data) override {
                    // cout << "recv_event generated" << '\n';
                    return new recv_event(_trigger_time, data);
                }

            public:
                std::string type() override { return "recv_event";}
                ~generator() override = default;
        };
        // this class is used to initialize the recv_event
        class recv_data{
            public:
                unsigned int s_id;
                unsigned int r_id;
                packet *_pkt;
        };

        // the recv_event::print() function is used for log file
        void print () const override {
            std::cout << "time "    << std::setw(11) << event::get_cur_time()
                << "   recID"       << std::setw(11) << receiverID
                << "   pktID"       << std::setw(11) << pkt->get_packet_ID()
                << "   srcID"       << std::setw(11) << pkt->get_header()->get_src_ID()
                << "   dstID"       << std::setw(11) << pkt->get_header()->get_dst_ID()
                << "   preID"       << std::setw(11) << pkt->get_header()->get_pre_ID()
                << "   nexID"       << std::setw(11) << pkt->get_header()->get_nex_ID()
                << "   "            << pkt->type()
                << pkt->addition_information();
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
recv_event::generator recv_event::generator::sample;

class send_event: public event {
    public:
        class send_data; // forward declaration

    private:
        send_event (send_event &){}
        send_event () = default; // we don't allow users to new a recv_event by themselves
        // this constructor cannot be directly called by users; only by generator
        unsigned int senderID; // the sender
        unsigned int receiverID; // the receiver
        packet *pkt; // the packet
        STATIC_CONSTRUCTOR (
            derived_class_names.emplace_back("send_event");
        )

    protected:
        send_event (unsigned int _trigger_time, const send_data &data): event(_trigger_time), senderID(data.s_id), receiverID(data.r_id), pkt(data._pkt){}

    public:
        ~send_event() override = default;
        // send_event will trigger the send function
        void trigger() override {
            if (pkt == nullptr) {
                std::cerr << "send_event error: no pkt!" << '\n';
                return ;
            }
            if (node::id_to_node(senderID) == nullptr){
                std::cerr << "send_event error: no node " << senderID << "!" << '\n';
                delete pkt; return ;
            }
            node::id_to_node(senderID)->send(pkt);
        }

        unsigned int event_priority() const override {
            std::string string_for_hash;
            string_for_hash = std::to_string(get_trigger_time()) + std::to_string(senderID) + std::to_string (receiverID) + std::to_string (pkt->get_packet_ID());
            return get_hash_value(string_for_hash);
        }

        // send_event::generator is derived from event::generator to generate a event
        class generator : public event::generator<send_data>{
                static generator sample;
                // this constructor is only for sample to register this event type
                generator() { /*cout << "send_event registered" << '\n';*/ register_event_type(&sample); }
            protected:
                event * generate(unsigned int _trigger_time, const send_data &data) override {
                    // cout << "send_event generated" << '\n';
                    return new send_event(_trigger_time, data);
                }

            public:
                std::string type() override { return "send_event";}
                ~generator() override = default;
        };
        // this class is used to initialize the send_event
        class send_data{
            public:
                unsigned int s_id;
                unsigned int r_id;
                packet *_pkt;
                unsigned int t;
        };

        void print () const override { // the send_event::print() function is used for log file
            std::cout << "time "     << std::setw(11) << event::get_cur_time()
                << "   senID"       << std::setw(11) << senderID
                << "   pktID"       << std::setw(11) << pkt->get_packet_ID()
                << "   srcID"       << std::setw(11) << pkt->get_header()->get_src_ID()
                << "   dstID"       << std::setw(11) << pkt->get_header()->get_dst_ID()
                << "   preID"       << std::setw(11) << pkt->get_header()->get_pre_ID()
                << "   nexID"       << std::setw(11) << pkt->get_header()->get_nex_ID()
                << "   "            << pkt->type()
                << pkt->addition_information()
                // << "   msg"         << setw(11) << dynamic_cast<IoT_data_payload*>(pkt->get_payload())->getMsg()
                << '\n';
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
send_event::generator send_event::generator::sample;

////////////////////////////////////////////////////////////////////////////////

class IoT_data_pkt_gen_event: public event {
    public:
        class gen_data; // forward declaration

        // this class is used to initialize the IoT_data_pkt_gen_event
        class pkt_gen_data{
            public:
                unsigned int src_id;
                unsigned int dst_id;
                std::string msg;
                // packet *_pkt;
        };

    private:
        IoT_data_pkt_gen_event (IoT_data_pkt_gen_event &){}
        IoT_data_pkt_gen_event () = default; // we don't allow users to new a IoT_data_pkt_gen_event by themselves
        // this constructor cannot be directly called by users; only by generator
        unsigned int src; // the src
        unsigned int dst; // the dst
        // packet *pkt; // the packet
        std::string msg;
        STATIC_CONSTRUCTOR (
            derived_class_names.emplace_back("IoT_data_pkt_gen_event");
        )

    protected:
        IoT_data_pkt_gen_event (unsigned int _trigger_time, const pkt_gen_data &data): event(_trigger_time), src(data.src_id), dst(data.dst_id), msg(data.msg){}

    public:
        ~IoT_data_pkt_gen_event() override = default;
        // IoT_data_pkt_gen_event will trigger the packet gen function
        void trigger() override {
            if (node::id_to_node(src) == nullptr){
                std::cerr << "IoT_data_pkt_gen_event error: no node " << src << "!" << '\n';
                return ;
            }
            if ( dst != BROADCAST_ID && node::id_to_node(dst) == nullptr ) {
                std::cerr << "IoT_data_pkt_gen_event error: no node " << dst << "!" << '\n';
                return;
            }

            IoT_data_packet *pkt = dynamic_cast<IoT_data_packet*> ( packet::generator::generate("IoT_data_packet") );
            if (pkt == nullptr) {
                std::cerr << "packet type is incorrect" << '\n'; return;
            }
            IoT_data_header *hdr = dynamic_cast<IoT_data_header*> ( pkt->get_header() );
            IoT_data_payload *pld = dynamic_cast<IoT_data_payload*> ( pkt->get_payload() );

            if (hdr == nullptr) {
                std::cerr << "header type is incorrect" << '\n'; return ;
            }
            if (pld == nullptr) {
                std::cerr << "payload type is incorrect" << '\n'; return ;
            }

            hdr->set_src_ID(src);
            hdr->set_dst_ID(dst);
            hdr->set_pre_ID(src); // this column is not important when the packet is first received by the src (i.e., just generated)
            hdr->set_nex_ID(src); // this column is not important when the packet is first received by the src (i.e., just generated)

            pld->set_msg(msg);

            recv_event::recv_data e_data;
            e_data.s_id = src;
            e_data.r_id = src; // to make the packet start from the src
            e_data._pkt = pkt;

            recv_event *e = dynamic_cast<recv_event*> ( event::generator<recv_event::recv_data>::generate("recv_event", trigger_time, e_data) );
        }

        unsigned int event_priority() const override {
            std::string string_for_hash;
            string_for_hash = std::to_string(get_trigger_time()) + std::to_string(src) + std::to_string (dst) ; //to_string (pkt->get_packet_ID());
            return get_hash_value(string_for_hash);
        }

        // IoT_data_pkt_gen_event::generator is derived from event::generator to generate an event
        class generator : public event::generator<pkt_gen_data>{
                static generator sample;
                // this constructor is only for sample to register this event type
                generator() { /*cout << "send_event registered" << '\n';*/ register_event_type(&sample); }
            protected:
                event * generate(unsigned int _trigger_time, const pkt_gen_data &data) override {
                    // cout << "send_event generated" << '\n';
                    return new IoT_data_pkt_gen_event(_trigger_time, data);
                }

            public:
                std::string type() override { return "IoT_data_pkt_gen_event";}
                ~generator() override = default;
        };

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
IoT_data_pkt_gen_event::generator IoT_data_pkt_gen_event::generator::sample;

class IoT_ctrl_pkt_gen_event: public event {
    public:
        class gen_data; // forward declaration

        // this class is used to initialize the IoT_ctrl_pkt_gen_event
        class pkt_gen_data{
            public:
                unsigned int src_id; // the sink
                unsigned int dst_id; // the node that should update its rule
                // unsigned int mat_id; // the target of the rule
                // unsigned int act_id; // the next hop toward the target recorded in the rule
                std::string msg;
                // double per; // the percentage
                // packet *_pkt;
        };

    private:
        IoT_ctrl_pkt_gen_event (IoT_ctrl_pkt_gen_event &){}
        IoT_ctrl_pkt_gen_event () = default; // we don't allow users to new a IoT_ctrl_pkt_gen_event by themselves
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

    protected:
        IoT_ctrl_pkt_gen_event (unsigned int _trigger_time, const pkt_gen_data &data): event(_trigger_time), src(data.src_id), dst(data.dst_id), msg(data.msg){}

    public:
        ~IoT_ctrl_pkt_gen_event() override = default;
        // IoT_ctrl_pkt_gen_event will trigger the packet gen function
        void trigger() override {
            IoT_ctrl_packet *pkt = dynamic_cast<IoT_ctrl_packet*> ( packet::generator::generate("IoT_ctrl_packet") );
            if (pkt == nullptr) {
                std::cerr << "packet type is incorrect" << '\n'; return;
            }
            IoT_ctrl_header *hdr = dynamic_cast<IoT_ctrl_header*> ( pkt->get_header() );
            IoT_ctrl_payload *pld = dynamic_cast<IoT_ctrl_payload*> ( pkt->get_payload() );

            if (hdr == nullptr) {
                std::cerr << "header type is incorrect" << '\n'; return ;
            }
            if (pld == nullptr) {
                std::cerr << "payload type is incorrect" << '\n'; return ;
            }

            hdr->set_src_ID(src);
            hdr->set_dst_ID(dst);
            hdr->set_pre_ID(src);
            hdr->set_nex_ID(src);

            // payload
            pld->set_msg(msg);
            // pld->setMatID(mat);
            // pld->setActID(act);
            // pld->setPer(per);

            recv_event::recv_data e_data;
            e_data.s_id = src;
            e_data.r_id = src;
            e_data._pkt = pkt;

            recv_event *e = dynamic_cast<recv_event*> ( event::generator<recv_event::recv_data>::generate("recv_event",trigger_time, e_data));
        }

        unsigned int event_priority() const override {
            std::string string_for_hash;
            // string_for_hash = to_string(get_trigger_time()) + to_string(src) + to_string(dst) + to_string(mat) + to_string(act); //to_string (pkt->get_packet_ID());
            string_for_hash = std::to_string(get_trigger_time()) + std::to_string(src) + std::to_string(dst) ; //to_string (pkt->get_packet_ID());
            return get_hash_value(string_for_hash);
        }

        // IoT_ctrl_pkt_gen_event::generator is derived from event::generator to generate an event
        class generator : public event::generator<pkt_gen_data>{
                static generator sample;
                // this constructor is only for sample to register this event type
                generator() { /*cout << "send_event registered" << '\n';*/ register_event_type(&sample); }
            protected:
                event * generate(unsigned int _trigger_time, const pkt_gen_data &data) override {
                    // cout << "send_event generated" << '\n';
                    return new IoT_ctrl_pkt_gen_event(_trigger_time, data);
                }

            public:
                std::string type() override { return "IoT_ctrl_pkt_gen_event";}
                ~generator() override = default;
        };

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
IoT_ctrl_pkt_gen_event::generator IoT_ctrl_pkt_gen_event::generator::sample;

class AGG_ctrl_pkt_gen_event: public event {
    public:
        class gen_data; // forward declaration

        // this class is used to initialize the AGG_ctrl_pkt_gen_event
        class pkt_gen_data{
            public:
                unsigned int src_id; // the sink
                unsigned int dst_id; // the node that should update its rule
                // unsigned int mat_id; // the target of the rule
                // unsigned int act_id; // the next hop toward the target recorded in the rule
                std::string msg;
                // double per; // the percentage
                // packet *_pkt;
        };

    private:
        AGG_ctrl_pkt_gen_event (AGG_ctrl_pkt_gen_event &){}
        AGG_ctrl_pkt_gen_event () = default; // we don't allow users to new a AGG_ctrl_pkt_gen_event by themselves
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

    protected:
        AGG_ctrl_pkt_gen_event (unsigned int _trigger_time, const pkt_gen_data &data): event(_trigger_time), src(data.src_id), dst(data.dst_id), msg(data.msg){}

    public:
        ~AGG_ctrl_pkt_gen_event() override = default;
        // AGG_ctrl_pkt_gen_event will trigger the packet gen function
        void trigger() override {
            AGG_ctrl_packet *pkt = dynamic_cast<AGG_ctrl_packet*> ( packet::generator::generate("AGG_ctrl_packet") );
            if (pkt == nullptr) {
                std::cerr << "packet type is incorrect" << '\n'; return;
            }
            AGG_ctrl_header *hdr = dynamic_cast<AGG_ctrl_header*> ( pkt->get_header() );
            AGG_ctrl_payload *pld = dynamic_cast<AGG_ctrl_payload*> ( pkt->get_payload() );

            if (hdr == nullptr) {
                std::cerr << "header type is incorrect" << '\n'; return ;
            }
            if (pld == nullptr) {
                std::cerr << "payload type is incorrect" << '\n'; return ;
            }

            hdr->set_src_ID(src);
            hdr->set_dst_ID(dst);
            hdr->set_pre_ID(src);
            hdr->set_nex_ID(src);

            // payload
            pld->set_msg(msg);
            // pld->setMatID(mat);
            // pld->setActID(act);
            // pld->setPer(per);

            recv_event::recv_data e_data;
            e_data.s_id = src;
            e_data.r_id = src;
            e_data._pkt = pkt;

            recv_event *e = dynamic_cast<recv_event*> ( event::generator<recv_event::recv_data>::generate("recv_event",trigger_time, e_data));
        }

        unsigned int event_priority() const override {
            std::string string_for_hash;
            // string_for_hash = to_string(get_trigger_time()) + to_string(src) + to_string(dst) + to_string(mat) + to_string(act); //to_string (pkt->get_packet_ID());
            string_for_hash = std::to_string(get_trigger_time()) + std::to_string(src) + std::to_string(dst) ; //to_string (pkt->get_packet_ID());
            return get_hash_value(string_for_hash);
        }

        // AGG_ctrl_pkt_gen_event::generator is derived from event::generator to generate an event
        class generator : public event::generator<pkt_gen_data>{
                static generator sample;
                // this constructor is only for sample to register this event type
                generator() { /*cout << "send_event registered" << '\n';*/ register_event_type(&sample); }
            protected:
                event * generate(unsigned int _trigger_time, const pkt_gen_data &data) override {
                    // cout << "send_event generated" << '\n';
                    return new AGG_ctrl_pkt_gen_event(_trigger_time, data);
                }

            public:
                std::string type() override { return "AGG_ctrl_pkt_gen_event";}
                ~generator() override = default;
        };

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
AGG_ctrl_pkt_gen_event::generator AGG_ctrl_pkt_gen_event::generator::sample;

class DIS_ctrl_pkt_gen_event: public event {
    public:
        class gen_data; // forward declaration

        // this class is used to initialize the DIS_ctrl_pkt_gen_event
        class pkt_gen_data{
            public:
                unsigned int src_id; // the sink
                unsigned int dst_id; // the node that should update its rule
                // unsigned int mat_id; // the target of the rule
                // unsigned int act_id; // the next hop toward the target recorded in the rule
                unsigned int parent;
                std::string msg;
                // double per; // the percentage
                // packet *_pkt;
        };

    private:
        DIS_ctrl_pkt_gen_event (DIS_ctrl_pkt_gen_event &){}
        DIS_ctrl_pkt_gen_event () = default; // we don't allow users to new a DIS_ctrl_pkt_gen_event by themselves
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

    protected:
        DIS_ctrl_pkt_gen_event (unsigned int _trigger_time, const pkt_gen_data &data): event(_trigger_time), src(data.src_id), dst(data.dst_id), msg(data.msg), parent(data.parent){}

    public:
        ~DIS_ctrl_pkt_gen_event() override = default;
        // DIS_ctrl_pkt_gen_event will trigger the packet gen function
        void trigger() override {
            DIS_ctrl_packet *pkt = dynamic_cast<DIS_ctrl_packet*> ( packet::generator::generate("DIS_ctrl_packet") );
            if (pkt == nullptr) {
                std::cerr << "packet type is incorrect" << '\n'; return;
            }
            DIS_ctrl_header *hdr = dynamic_cast<DIS_ctrl_header*> ( pkt->get_header() );
            DIS_ctrl_payload *pld = dynamic_cast<DIS_ctrl_payload*> ( pkt->get_payload() );

            if (hdr == nullptr) {
                std::cerr << "header type is incorrect" << '\n'; return ;
            }
            if (pld == nullptr) {
                std::cerr << "payload type is incorrect" << '\n'; return ;
            }

            hdr->set_src_ID(src);
            hdr->set_dst_ID(dst);
            hdr->set_pre_ID(src);
            hdr->set_nex_ID(src);

            // payload
            pld->set_parent(parent);
            pld->set_msg(msg);
            // pld->setMatID(mat);
            // pld->setActID(act);
            // pld->setPer(per);

            recv_event::recv_data e_data;
            e_data.s_id = src;
            e_data.r_id = src;
            e_data._pkt = pkt;

            recv_event *e = dynamic_cast<recv_event*> ( event::generator<recv_event::recv_data>::generate("recv_event",trigger_time, e_data));
        }

        unsigned int event_priority() const override {
            std::string string_for_hash;
            // string_for_hash = to_string(get_trigger_time()) + to_string(src) + to_string(dst) + to_string(mat) + to_string(act); //to_string (pkt->get_packet_ID());
            string_for_hash = std::to_string(get_trigger_time()) + std::to_string(src) + std::to_string(dst) ; //to_string (pkt->get_packet_ID());
            return get_hash_value(string_for_hash);
        }

        // DIS_ctrl_pkt_gen_event::generator is derived from event::generator to generate an event
        class generator : public event::generator<pkt_gen_data>{
                static generator sample;
                // this constructor is only for sample to register this event type
                generator() { /*cout << "send_event registered" << '\n';*/ register_event_type(&sample); }
            protected:
                event * generate(unsigned int _trigger_time, const pkt_gen_data &data) override {
                    // cout << "send_event generated" << '\n';
                    return new DIS_ctrl_pkt_gen_event(_trigger_time, data);
                }

            public:
                std::string type() override { return "DIS_ctrl_pkt_gen_event";}
                ~generator() override = default;
        };

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
DIS_ctrl_pkt_gen_event::generator DIS_ctrl_pkt_gen_event::generator::sample;

////////////////////////////////////////////////////////////////////////////////

class link {
        // all links created in the program
        static inline std::map< std::pair<unsigned int,unsigned int>, link*> id_id_link_table;
        unsigned int id1; // from
        unsigned int id2; // to

    protected:
        link(link&){} // this constructor should not be used
        link() = default; // this constructor should not be used
        link(unsigned int _id1, unsigned int _id2): id1(_id1), id2(_id2) { id_id_link_table[std::pair<unsigned int,unsigned int>(id1,id2)] = this; }
        static inline std::vector<std::string> derived_class_names;

    public:
        virtual ~link() {
            id_id_link_table.erase (std::pair<unsigned int,unsigned int>(id1,id2)); // erase the link
        }

        static link * id_id_to_link (unsigned int _id1, unsigned int _id2) {
            return ((id_id_link_table.find(std::pair<unsigned int,unsigned int>(_id1, _id2)) != id_id_link_table.end()) ? id_id_link_table[std::pair<unsigned,unsigned>(_id1,_id2)] : nullptr);
        }

        virtual double get_latency() = 0; // you must implement your own latency

        static void del_link (unsigned int _id1, unsigned int _id2) {
            std::pair<unsigned int, unsigned int> temp;
            if (id_id_link_table.find(temp)!=id_id_link_table.end()) {
                id_id_link_table.erase(temp);
            }
        }

        static unsigned int get_link_num () { return id_id_link_table.size(); }

        static void print () {
            std::cout << "registered link types:\n";
            for (const auto &name: derived_class_names) {
                std::cout << name << '\n';
            }
        }

        class generator {
                // lock the copy constructor
                generator(generator &) = default;
                // store all possible types of link
                static inline std::map<std::string, generator*> prototypes;
            protected:
                // allow derived class to use it
                generator() = default;
                // after you create a new link type, please register the factory of this link type by this function
                void register_link_type(generator *h) { prototypes[h->type()] = h; }
                // you have to implement your own generate() to generate your link
                virtual link* generate(unsigned int _id1, unsigned int _id2) = 0;
            public:
                // you have to implement your own type() to return your link type
        	    virtual std::string type() = 0;
        	    // this function is used to generate any type of link derived
        	    static link * generate (const std::string &type, unsigned int _id1, unsigned int _id2) {
        	        if(id_id_link_table.find(std::pair<unsigned int, unsigned int>(_id1, _id2)) != id_id_link_table.end()){
        	            std::cerr << "duplicate link id" << '\n'; // link id is duplicated
        	            return nullptr;
        	        }
        	        if ( BROADCAST_ID == _id1 || BROADCAST_ID == _id2 ) {
        	            std::cerr << "BROADCAST_ID cannot be used" << '\n';
        	            return nullptr;
        	        }
            		if (prototypes.find(type) != prototypes.end()){ // if this type derived exists
            		    link * created_link = prototypes[type]->generate(_id1,_id2);
            			return created_link; // generate it!!
            		}
            		std::cerr << "no such link type" << '\n'; // otherwise
            		return nullptr;
            	}
            	virtual ~generator() = default;
        };
};

void node::add_phy_neighbor (unsigned int _id, const std::string &link_type){
    if (id == _id) {return;} // if the two nodes are the same...
    if (id_node_table.find(_id)==id_node_table.end()) {return;} // if this node does not exist
    if (phy_neighbors.find(_id)!=phy_neighbors.end()) {return;} // if this neighbor has been added
    phy_neighbors[_id] = true;

    link::generator::generate(link_type,id,_id);
}

class simple_link: public link {
    private:
        STATIC_CONSTRUCTOR (
            derived_class_names.emplace_back("simple_link");
        )
    protected:
        simple_link() = default; // it should not be used outside the class
        simple_link(simple_link&) {} // it should not be used
        simple_link(unsigned int _id1, unsigned int _id2): link (_id1,_id2){} // this constructor cannot be directly called by users

    public:
        ~simple_link() override = default;
        double get_latency() override { return ONE_HOP_DELAY; } // you can implement your own latency

        // simple_link::generator is derived from link::generator to generate a link
        class generator : public link::generator {
                static generator sample;
                // this constructor is only for sample to register this link type
                generator() { /*cout << "simple_link registered" << '\n';*/ register_link_type(&sample); }
            protected:
                link * generate(unsigned int _id1, unsigned int _id2) override
                { /*cout << "simple_link generated" << '\n';*/ return new simple_link(_id1,_id2); }
            public:
                std::string type() override { return "simple_link"; }
                ~generator() override = default;
        };
};

simple_link::generator simple_link::generator::sample;


// the IoT_data_packet_event function is used to add an initial event
void IoT_data_packet_event (unsigned int src, unsigned int dst=0, unsigned int t = 0, std::string msg = "default"){
    if ( node::id_to_node(src) == nullptr || (dst != BROADCAST_ID && node::id_to_node(dst) == nullptr) ) {
        std::cerr << "src or dst is incorrect" << '\n';
        return;
    }

    IoT_data_pkt_gen_event::pkt_gen_data e_data;
    e_data.src_id = src;
    e_data.dst_id = dst;
    e_data.msg = msg;

    // recv_event *e = dynamic_cast<recv_event*> ( event::generator::generate("recv_event",t, (void *)&e_data) );
    IoT_data_pkt_gen_event *e = dynamic_cast<IoT_data_pkt_gen_event*>(event::generator<IoT_data_pkt_gen_event::pkt_gen_data>::generate("IoT_data_pkt_gen_event", t, e_data));
    if (e == nullptr) {
        std::cerr << "event type is incorrect" << '\n';
    }
}

// the IoT_ctrl_packet_event function is used to add an initial event

void IoT_ctrl_packet_event (unsigned int src = 0, unsigned int t = event::get_cur_time(),
                        std::string msg = "default") {
        // 1st parameter: the source; the destination that want to broadcast a msg with counter 0 (i.e., match ID)
        // 2nd parameter: time (optional)
        // 3rd parameter: msg (optional)
    if ( node::id_to_node(src) == nullptr ) {
        std::cerr << "id is incorrect" << '\n'; return;
    }

    // unsigned int src = con_id;
    IoT_ctrl_pkt_gen_event::pkt_gen_data e_data;
    e_data.src_id = src;
    e_data.dst_id = BROADCAST_ID;
    // e_data.mat_id = mat;
    // e_data.act_id = act;
    e_data.msg = msg;
    // e_data.per = per;

    IoT_ctrl_pkt_gen_event *e = dynamic_cast<IoT_ctrl_pkt_gen_event*>(event::generator<IoT_ctrl_pkt_gen_event::pkt_gen_data>::generate("IoT_ctrl_pkt_gen_event", t, e_data));
    if (e == nullptr) {
        std::cerr << "event type is incorrect" << '\n';
    }
}

// the AGG_ctrl_packet_event function is used to add an initial event
void AGG_ctrl_packet_event (unsigned int src, unsigned int dst = 0, unsigned int t = event::get_cur_time(), std::string msg = "default"){
    if ( node::id_to_node(src) == nullptr || (dst != BROADCAST_ID && node::id_to_node(dst) == nullptr) ) {
        std::cerr << "src or dst is incorrect" << '\n';
        return;
    }

    AGG_ctrl_pkt_gen_event::pkt_gen_data e_data;
    e_data.src_id = src;
    e_data.dst_id = dst;
    e_data.msg = msg;

    // recv_event *e = dynamic_cast<recv_event*> ( event::generator::generate("recv_event",t, (void *)&e_data) );
    AGG_ctrl_pkt_gen_event *e = dynamic_cast<AGG_ctrl_pkt_gen_event*>(event::generator<AGG_ctrl_pkt_gen_event::pkt_gen_data>::generate("AGG_ctrl_pkt_gen_event", t, e_data));
    if (e == nullptr) {
        std::cerr << "event type is incorrect" << '\n';
    }
}

// the DIS_ctrl_packet_event function is used to add an initial event
void DIS_ctrl_packet_event (unsigned int sink_id = 0, unsigned int t = event::get_cur_time(), const std::string &msg = "default"){
    if ( node::id_to_node(sink_id) == nullptr ) {
        std::cerr << "sink_id or id is incorrect" << '\n';
        return;
    }

    DIS_ctrl_pkt_gen_event::pkt_gen_data e_data;
    e_data.src_id = sink_id;
    e_data.dst_id = sink_id;
    e_data.parent = sink_id;
    e_data.msg = msg;
    
    // recv_event *e = dynamic_cast<recv_event*> ( event::generator::generate("recv_event",t, (void *)&e_data) );
    DIS_ctrl_pkt_gen_event *e = dynamic_cast<DIS_ctrl_pkt_gen_event*>(event::generator<DIS_ctrl_pkt_gen_event::pkt_gen_data>::generate("DIS_ctrl_pkt_gen_event", t, e_data));
    if (e == nullptr) {
        std::cerr << "event type is incorrect" << '\n';
    }
}

// send_handler function is used to transmit packet p based on the information in the header
// Note that the packet p will not be discard after send_handler ()

void node::send_handler(packet *p){
    packet *_p = packet::generator::replicate(p);
    send_event::send_data e_data;
    e_data.s_id = _p->get_header()->get_pre_ID();
    e_data.r_id = _p->get_header()->get_nex_ID();
    e_data._pkt = _p;
    send_event *e = dynamic_cast<send_event*> (event::generator<send_event::send_data>::generate("send_event", event::get_cur_time(), e_data));
    if (e == nullptr) {
        std::cerr << "event type is incorrect" << '\n';
    }
}

void node::send(packet *p){ // this function is called by event; not for the user
    if (p == nullptr) {return;}

    unsigned int _nexID = p->get_header()->get_nex_ID();
    for (const auto &[nb_id, _]: phy_neighbors) { // neighbor id
        if (nb_id != _nexID && BROADCAST_ID != _nexID) {continue;} // this neighbor will not receive the packet

        unsigned int trigger_time = event::get_cur_time() + link::id_id_to_link(id, nb_id)->get_latency() ; // we simply assume that the delay is fixed
        // cout << "node " << id << " send to node " <<  nb_id << '\n';
        recv_event::recv_data e_data;
        e_data.s_id = id;    // set the sender   (i.e., preID)
        e_data.r_id = nb_id; // set the receiver (i.e., nexID)

        packet *p2 = packet::generator::replicate(p);
        e_data._pkt = p2;

        recv_event *e = dynamic_cast<recv_event*> (event::generator<recv_event::recv_data>::generate("recv_event", trigger_time, e_data)); // send the packet to the neighbor
        if (e == nullptr) {
            std::cerr << "event type is incorrect" << '\n';
        }
    }
    packet::discard(p);
}

int main()
{
    // header::generator::print(); // print all registered headers
    // payload::generator::print(); // print all registered payloads
    // packet::generator::print(); // print all registered packets
    // node::generator::print(); // print all registered nodes
    // event::generator::print(); // print all registered events
    // link::generator::print(); // print all registered links

    // read the input and generate devices
    for (unsigned int id = 0; id < 5; id ++){

        node::generator::generate("IoT_device",id);

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
