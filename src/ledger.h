/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef LEDGER_H_
#define LEDGER_H_

#include <string>
#include "bat_helper.h"

namespace bat_client {
  class BatClient;
}

namespace bat_publisher {
  class BatPublisher;
}

namespace ledger {

class Ledger {
public:
  Ledger();
  ~Ledger();

  void createWallet();
  void initSynopsis();
  void saveVisit(const std::string& publisher, const uint64_t& duration);
  void saveVisitCallback(const std::string& publisher, const uint64_t& verifiedTimestamp);
  void favIconUpdated(const std::string& publisher, const std::string& favicon_url);
  void setPublisherInclude(const std::string& publisher, const bool& include);
  void setPublisherDeleted(const std::string& publisher, const bool& deleted);
  void setPublisherPinPercentage(const std::string& publisher, const bool& pinPercentage);
  void setPublisherMinVisitTime(const uint64_t& duration); // In milliseconds
  void setPublisherMinVisits(const unsigned int& visits);
  void setPublisherAllowNonVerified(const bool& allow);
  void setContributionAmount(const double& amount);

  std::string getBATAddress();
  std::string getBTCAddress();
  std::string getETHAddress();
  std::string getLTCAddress();

  void getBalance();

private:
  bool isBatClientExist();
  bool isBatPublisherExist();
  void publisherInfoCallback(bool result, const std::string& response, const FETCH_CALLBACK_EXTRA_DATA_ST& extraData);
  void walletPropertiesCallback(bool result, const std::string& response, const FETCH_CALLBACK_EXTRA_DATA_ST& extraData);

  bat_client::BatClient* bat_client_;
  bat_publisher::BatPublisher* bat_publisher_;
};
}

#endif  // LEDGER_H_
