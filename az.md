# Ledger Zenon App

Zenon wallet application for Ledger

**Team:** [HyperCore-One](https://github.com/hypercore-one)  
**Repository**: [ledger-app-zenon](https://github.com/hypercore-one/ledger-app-zenon)  
**Proposal**: [Accelerator-Z](https://github.com/hypercore-one/ledger-app-zenon/az.md)

## Introduction

The s y r i u s desktop wallet is the official wallet of the Zenon cryptocurrency. The s y r i u s desktop wallet is a software wallet for storing the Zenon cryptocurrency on a PC. The information is not accessible to anyone but the user, whose private keys are kept only on the desktop. Storing private keys on a PC entails the necessary risks.

As the Zenon network grows, node operators and users will need more secure alternatives for storing private keys. Pillar owners for example have a large stake in the network and are at the greatest risk of losing their private keys. Pillar owners and users alike will greatly benefit from being able to store their private keys on a hardware wallet.

Hardware wallets are devices specially designed to securely store private keys. They’re considered safer than desktop or smartphone wallets, mainly because they don’t connect to the Internet at any point. These properties significantly reduce the attack vectors available to malicious parties, as it means that they can’t tamper with the device remotely.

A good hardware wallet ensures that private keys never leave the device. They’re normally held in a special place in the device that doesn’t allow them to be removed.

The hardware wallet market is multifaceted with different players. This project specifically focuses on a wallet application for Ledger. Ledger is one of the biggest players in the market and one of the few with good open-source support for developers. Not only is Ledger the only hardware wallet provider that has created their own Operating System, they’re equally one of the few that make use of an application system. Other hardware wallets use what is known as a monolithic firmware. In these, if there’s a problem with the code for one cryptocurrency, it can affect all others. 

Alternate players like Trezor specifically specify that they don’t offer coin-adding abilities at this time. Others have strict requirements for adding coins, such as a top 30 coin, a bitcoin or ERC20 derivative coin or extensive partnership programs. In most cases, the development for adding additional coins are done by the team of the respective company and not through open-source possibilities like Ledger. In addition, it is the intention to raise the safest technology with the largest user base in the shortest possible time frame. Ledger meets these requirements and stands head and shoulders above the other players.

## Ledger

Ledger offers various hardware wallets for developing Embedded applications. The Embedded applications are written in C (or RUST) on the Blockchain Open Ledger Operating System (BOLOS). The BOLOS application is the application that allows users to manage their crypto-assets with their Ledger devices. All Ledger devices run on BOLOS. This means Embedded applications can be easily ported to other Ledger devices.

The development of the application consists of different release types, namely:

- Developer mode release
	- Unaudited application
	- Audited application
- Public release

Unaudited or audited developer mode applications are considered experimental and such application will be visible **only when the  developer mode setting is enabled** in Ledger Live settings.

Audited Developer mode releases and Public releases have been audited by the Ledger security team.

These are the reviews that Ledger does, according per release type:

| Type of review | Unaudited | Audited | Public |
|--|--|--|--|
| Security audit | | :heavy_check_mark: | :heavy_check_mark: |
| Functional | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |
| Documentation | | | :heavy_check_mark: |
| Functional | | | :heavy_check_mark: |
| Marketing plan | | | :heavy_check_mark: |
| Official Ledger<br/>Support | | | :heavy_check_mark: |


The Public release is subject to strict requirements and goes through several steps, such as security audits, complete documentation, marketing plan, configured github repository according to guidelines.

The security audit process should be started only when the Embedded App is ready for all Ledger devices (Ledger Nano S, S Plus and Stax). A full list of all release steps can be found under [user case 4](https://developers.ledger.com/docs/embedded-app/case4/).

Ledger offers developers several use cases for developing an Embedded App. Zenon has an unique architecture with its own Token Standard. Zenon is not an ETH clone, and cannot be derived from an existing app. This means that use cases 3 and 5 will be dropped. External development is an option if development proves to be too challenging for community developers. A possible outsourcing would mean that the person concerned would first have to learn more about the Zenon architecture, which would probably entail extra unnecessary costs. This project however aimes to apply a combination of use cases 2 and 4.

Because of the aforementioned Ledger release types, it is only logical that the phases of this project follow the same order.

## Phases

The typical end-to-end architecture of a BOLOS application, in the case of an app for a cryptocurrency:

![app_architecture](/assets/diagrams/app-architecture.png)

Three pieces of software are required:

-   A BOLOS application,
-   The with business logic and typically a GUI, running on a computer or a phone connected to the Ledger device
-   A transaction explorer that interfaces between the front-end and the cryptocurrency daemon, running either locally (in the case of a full node) or remotely (light wallet)

The Ledger Manager is the service which centralizes the distribution of BOLOS applications on Ledger devices. It is the place where both Ledger’s apps and third party apps are distributed to users.

This project is specifically for the development of the BOLOS application and will implement a CLI integration for the Zenon CLI. The CLI implementation is for testing purposes only.

The phases of this project follow the same order as the Ledger release types.

- Phase 1 = unaudited developer mode release
- Phase 2 = audited developer mode release
- Phase 3 = public release

## Phase 1 - Unaudited developer mode release

The first phase of this project focuses on the development of an unaudited developer mode release for the Ledger Nano S.

The Ledger Nano S was the first device released by Ledger and is therefore the most widely available. A physical Ledger Nano S and Ledger Nano S Plus will be made available and used for the development of the application. 

Ledger also offers device simulation with the Speculos emulator. The Speculos emulates Ledger hardware wallets (Nano S, Nano X, Nano S Plus and Blue) on desktop computers. This is particularly useful when to facilitate the pressing of Nano buttons.

**Application architecture**

The main goal of the application is designed to sign a NoM transaction and based on the preliminary research, the application will implement the following UI flows:

**App Info UI flow**

![app-info-flow](/assets/diagrams/app-flow-info.png)

**Send UI Flow**

![app-send-flow](/assets/diagrams/app-flow-send.png)

Unlike other cryptocurrencies, Zenon requires unreceived transactions to be signed. The send flow is applicable for both transaction types. Signing unreceived transactions or interacting with Embedded Smart Contracts will possibly require a [blind signing](https://developers.ledger.com/docs/embedded-app/functional-requirements/) setting.

**Receive UI Flow**

![app-receive-flow](/assets/diagrams/app-flow-receive.png)

**Security**

Ledger devices are Hierarchical Deterministic wallets, which means they can derive as many keys as necessary from a single seed as defined in [BIP-0032](https://github.com/bitcoin/bips/blob/master/bip-0032.mediawiki). The application will use the following settings:

- Derivation path = m/44'/73404
- Curve = ed25519

**Transaction signature flow**

![app-tx-flow](/assets/diagrams/app-flow-tx.png)

- **Receive**: the transaction is sent through one or more APDUs. If this transaction is too large to be sent at once, it is copied chunk by chunk into a global buffer.
- **Parsing**: the various fields of the transaction are read. This step is devoted to making sure the transacation is correctly formed and extracting its fields. 
- **Selection**: when the transaction is deemed valid, its fields must be printed to the user.
- **Display**: This step iterates over those fields. Each of them must be made human-readable through formatting and then displayed on screen.
- **Signing**: if the user validates the transaction then it is signed and its signature is sent back to the user by the application.

**Anatomy of a transaction**

- Loading the transaction data;
- Presenting context with the transaction prompt;
- Reviewing the transaction data;
- Signing the transaction;
- and rejecting the transaction.

**High level overview of main tasks**

- Get in touch with the Ledger developer community
- Learn and set up the BOLOS environment
- Learn Ledger framework and SDKs
- Set up repository according to Ledger security guidelines (CICD)
- Design icons
- Design UI flows
- Define and implement APDUs
- Implement transaction signature flow
- Documentation
- Functional tests

**Completion of Phase 1 will be measured by**

- **Must have**: Sign a tx to send ZNN/QSR to another user. 
- **Must have**: Sign a tx to receive ZNN/QSR from another user.
- **Should have**: Sign a tx to send to an Embedded Smart Contract. 
- **Should have**: Sign a tx to receive from an Embedded Smart Contract. 
- **Could have**: Support for sending/receiving Zenon Token Standard tokens. 

## Phase 2 - Audited developer mode release

The second phase of this project focuses on the development of an audited developer mode release for all Ledger devices.

This fase requires additional funding outside the scope of this project to fund the security audit(s). The exact costs of the security audit(s) are uncertain at this stage.

This phase will be fully worked out when phase 1 is completed.

**Completion of Phase 1 will be measured by**

- **Must have**: Successfully pass the security audit.

## Phase 3 - Public release

The third phase of this project focuses on the development of a public release.

This phase will be fully worked out when phase 2 is completed.

## Funding

**Total Requested Funding** = 5000 ZNN and 50000 QSR
**Project Duration** = 384 hours

**How did you calculate your budget?** 

The realization of phases 2 and 3 largely depends on third parties and the market. For example, phase 2 is subject to the requirement that Zenon must be in the top 600 coins on cointmarketcap to meet the requirements for the security audit; futhermore, the security audit(s) most be approved and funded in a separate AZ project.

The estimate is therefore based only on the total expected time I will need to complete Phase 1 and additional work that lead up to the security audit for Phase 2. The expected duration of phase 1 is expected to be between 6 and 8 months.

I've calculated a ZNN & QSR of 2 and 0,20 dollar, which covers about 52% of the actual time spend against a rate of 100 dollar per hour. The remaining hours are considered learning hours for my own personal development.

The tools necessary for the job will be bought at own expense and are included in the budget.

## Additional projects outside the scope of this project

During the preliminary investigation, a number of projects were identified that fall outside the scope of this project. This list is not exhaustive and can be picked up by other community members.

**Support for s y r i u s desktop wallet**

Support for the s y r i u s wallet is considered outside the scope of this project. A separate project can start the integration process as soon as phase 1 of this project is complete. Communication is done over USB or Bluetooth using an APDU. The APDU functions as a common interface between the Ledger and Host device and will be defined and implemented in phase 1.

**Pillar migration**

Node operators have expressed the need to able to migrate their addresses. For complete support, node operators will need a way to move pillars from one address to another without dismantling them (and re-burning QSR, etc). Without this all existing pillars will forever stay in hot wallets.
