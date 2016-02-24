// Part of measurement-kit <https://measurement-kit.github.io/>.
// Measurement-kit is free software. See AUTHORS and LICENSE for more
// information on the copying conditions.

#ifndef SRC_OONI_DNS_INJECTION_HPP
#define SRC_OONI_DNS_INJECTION_HPP

#include <measurement_kit/dns.hpp>
#include "src/ooni/errors.hpp"
#include "src/ooni/ooni_test_impl.hpp"
#include "src/ooni/dns_test_impl.hpp"
#include <sys/stat.h>

namespace mk {
namespace ooni {

class DNSInjectionImpl : public DNSTestImpl {
    using DNSTestImpl::DNSTestImpl;

    std::function<void(report::Entry)> have_entry;

  public:
    DNSInjectionImpl(std::string input_filepath_, Settings options_)
        : DNSTestImpl(input_filepath_, options_) {
        test_name = "dns_injection";
        test_version = "0.0.1";

        if (input_filepath_ == "") {
            throw InputFileRequired("An input file is required!");
        }

        struct stat buffer;
        if (stat(input_filepath_.c_str(), &buffer) != 0) {
            throw InputFileDoesNotExist(input_filepath_ + " does not exist");
        }
    };

    void main(std::string input, Settings options,
              std::function<void(report::Entry)> &&cb) {
        entry["injected"] = NULL;
        have_entry = cb;
        query("A", "IN", input, options["nameserver"],
              [this](dns::Response response) {
                  logger.debug("dns_injection: got response");
                  if (response.get_evdns_status() == DNS_ERR_NONE) {
                      entry["injected"] = true;
                  } else {
                      entry["injected"] = false;
                  }
                  have_entry(entry);
              });
    }
};

} // namespace ooni
} // namespace mk
#endif