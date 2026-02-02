#ifndef HPP_ARGPARSER
#define HPP_ARGPARSER

#include <type_traits>
#include <stdexcept>
#include <vector>
#include <map>
#include "lexical_cast.hpp"

class ArgParser {
	private:
			enum class ArgKind {
				Flag,
				Value,
				OptionalValue
			};

		class IArgument {
			public:
				virtual ~IArgument() {}
				virtual void parse(const std::string & s) = 0;
		};

		template <typename T>
		class Argument : public IArgument {
			public:
				Argument(T & t) : m_t(t) {}
				~Argument() {}
	
				void parse(const std::string & s) {
					m_t = fromString<T>(s);
				}
	
			private:
				T & m_t;
		};

		template <typename T>
		class MultiArgument : public IArgument {
			public:
				MultiArgument(std::vector<T> & t) : m_t(t) {}
				MultiArgument() {}

				void parse(const std::string & s) {
					m_t.push_back(fromString<T>(s));
				}

			private:
				std::vector<T> & m_t;
		};

			template <typename T>
			class OptionalArgument : public IArgument {
				public:
					OptionalArgument(bool & b, T & t) : m_b(b), m_t(t) {}
					~OptionalArgument() {}

					void parse(const std::string & s) {
						m_b = true;
						if (!s.empty()) {
							m_t = fromString<T>(s);
						}
					}

				private:
					bool & m_b;
					T & m_t;
			};

	public:
		ArgParser(int argc, char * * argv) {
			for (int i = 1; i < argc; ++i) {
				m_args.push_back(argv[i]);
			}
		}

		~ArgParser() {
			for (auto & i : m_mapArgs) {
				delete i.second.second; // :)
			}
		}

		template <typename T>
		void addSwitch(const char switchShort, const std::string switchLong, T & t) {
			const std::string strShort = std::string("-") + switchShort;
			const std::string strLong = std::string("--") + switchLong;

			// :)
			IArgument * const pArgShort = new Argument<T>(t);
			IArgument * const pArgLong = new Argument<T>(t);
			m_mapArgs[strShort] = std::pair<ArgKind, IArgument *>(std::is_same<bool, T>::value ? ArgKind::Flag : ArgKind::Value, pArgShort);
			m_mapArgs[strLong] = std::pair<ArgKind, IArgument *>(std::is_same<bool, T>::value ? ArgKind::Flag : ArgKind::Value, pArgLong);
		}

		template <typename T>
		void addOptionalSwitch(const char switchShort, const std::string switchLong, bool & b, T & t) {
			const std::string strShort = std::string("-") + switchShort;
			const std::string strLong = std::string("--") + switchLong;

			IArgument * const pArgShort = new OptionalArgument<T>(b, t);
			IArgument * const pArgLong = new OptionalArgument<T>(b, t);
			m_mapArgs[strShort] = std::pair<ArgKind, IArgument *>(ArgKind::OptionalValue, pArgShort);
			m_mapArgs[strLong] = std::pair<ArgKind, IArgument *>(ArgKind::OptionalValue, pArgLong);
		}

		template <typename T>
		void addMultiSwitch(const char switchShort, const std::string switchLong, std::vector<T> & t) {
			const std::string strShort = std::string("-") + switchShort;
			const std::string strLong = std::string("--") + switchLong;

			// :)
			IArgument * const pArgShort = new MultiArgument<T>(t);
			IArgument * const pArgLong = new MultiArgument<T>(t);
			m_mapArgs[strShort] = std::pair<ArgKind, IArgument *>(ArgKind::Value, pArgShort);
			m_mapArgs[strLong] = std::pair<ArgKind, IArgument *>(ArgKind::Value, pArgLong);
		}

		bool parse() const {
			try {
				std::vector<std::string>::size_type i = 0;

				while (i < m_args.size()) {
					auto p = m_mapArgs.at(m_args[i]);

					switch (p.first) {
						case ArgKind::Flag:
							p.second->parse("1");
							i += 1;
							break;
						case ArgKind::Value:
							p.second->parse(m_args.at(i + 1));
							i += 2;
							break;
						case ArgKind::OptionalValue: {
							std::string s;
							if (i + 1 < m_args.size() && !m_args[i + 1].empty() && m_args[i + 1][0] != '-') {
								s = m_args[i + 1];
								i += 2;
							} else {
								s = "";
								i += 1;
							}
							p.second->parse(s);
							break;
						}
					}
				}
			}
			catch (std::out_of_range & e) {
				return false;
			}

			return true;
		}

	private:
		std::vector<std::string> m_args;
		std::map<std::string, std::pair<ArgKind, IArgument *>> m_mapArgs;
};

#endif /* HPP_ARGPARSER */