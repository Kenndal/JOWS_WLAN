/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2016 AGH University of Science and Technology
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as 
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * Author: Lukasz Prasnal <prasnal@kt.agh.edu.pl>
 */

#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/wifi-module.h"
#include "ns3/propagation-module.h"
#include "ns3/mobility-module.h"
#include "ns3/rng-seed-manager.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/ipv4-flow-classifier.h"

using namespace ns3; 

NS_LOG_COMPONENT_DEFINE ("wifi-qos-test");

class SimulationHelper {
public:
    SimulationHelper();

    static OnOffHelper
    CreateOnOffHelper(std::string proto, InetSocketAddress socketAddress, DataRate dataRate, int packetSize,
                      uint8_t tid, Time start, Time stop);

    static void PopulateArpCache();
};

SimulationHelper::SimulationHelper() {
}

//prepare CBR traffic source
OnOffHelper SimulationHelper::CreateOnOffHelper(std::string proto, InetSocketAddress socketAddress, DataRate dataRate,
                                                int packetSize, uint8_t tid, Time start, Time stop) {
    socketAddress.SetTos(tid
                                 << 5); //(see: https://www.tucny.com/Home/dscp-tos and http://www.revolutionwifi.net/revolutionwifi/2010/08/wireless-qos-part-3-user-priorities.html)

    OnOffHelper onOffHelper(proto, socketAddress);
    onOffHelper.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=100000]"));
    onOffHelper.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
    onOffHelper.SetAttribute("DataRate", DataRateValue(dataRate));
    onOffHelper.SetAttribute("PacketSize", UintegerValue(packetSize));
    //onOffHelper.SetAttribute ("Jitter",     DoubleValue (1.0)); //packets generation times modified by random value between -50% and +50% of constant time step between packets
    //onOffHelper.SetAttribute ("PoissonTraffic", BooleanValue (true)); //generate packets in Poisson process
    onOffHelper.SetAttribute("MaxBytes", UintegerValue(0));
    onOffHelper.SetAttribute("StartTime", TimeValue(start));
    onOffHelper.SetAttribute("StopTime", TimeValue(stop));

    return onOffHelper;
}

//fullfil the ARP cache prior to simulation run
void
SimulationHelper::PopulateArpCache () 
{
  Ptr<ArpCache> arp = CreateObject<ArpCache> ();
  arp->SetAliveTimeout (Seconds (3600 * 24 * 365) );
	
  for (NodeList::Iterator i = NodeList::Begin (); i != NodeList::End (); ++i) 
    {	
      Ptr<Ipv4L3Protocol> ip = (*i)->GetObject<Ipv4L3Protocol> ();
      NS_ASSERT (ip != 0);
      ObjectVectorValue interfaces;
      ip->GetAttribute ("InterfaceList", interfaces);

      for (ObjectVectorValue::Iterator j = interfaces.Begin (); j != interfaces.End (); j++) 
        {		
          Ptr<Ipv4Interface> ipIface = (*j).second->GetObject<Ipv4Interface> ();
          NS_ASSERT (ipIface != 0);
          Ptr<NetDevice> device = ipIface->GetDevice ();
          NS_ASSERT (device != 0);
          Mac48Address addr = Mac48Address::ConvertFrom (device->GetAddress () );
      
          for (uint32_t k = 0; k < ipIface->GetNAddresses (); k++) 
            {			
              Ipv4Address ipAddr = ipIface->GetAddress (k).GetLocal();		
              if (ipAddr == Ipv4Address::GetLoopback ()) 
                continue;

              ArpCache::Entry *entry = arp->Add (ipAddr);
              Ipv4Header ipv4Hdr;
              ipv4Hdr.SetDestination (ipAddr);
              Ptr<Packet> p = Create<Packet> (100);  
              entry->MarkWaitReply (ArpCache::Ipv4PayloadHeaderPair (p, ipv4Hdr) );
              entry->MarkAlive (addr);
            }
        }
    }

    for (NodeList::Iterator i = NodeList::Begin (); i != NodeList::End (); ++i) 
      {
        Ptr<Ipv4L3Protocol> ip = (*i)->GetObject<Ipv4L3Protocol> ();
		NS_ASSERT (ip != 0);
		ObjectVectorValue interfaces;
		ip->GetAttribute ("InterfaceList", interfaces);

        for (ObjectVectorValue::Iterator j = interfaces.Begin (); j != interfaces.End (); j ++)
          {
            Ptr<Ipv4Interface> ipIface = (*j).second->GetObject<Ipv4Interface> ();
            ipIface->SetAttribute ("ArpCache", PointerValue (arp) );
          }
      }
}



/* ===== main function ===== */

int main (int argc, char *argv[])
{
    uint32_t nSTA = 3;
    uint32_t packetSize = 1470;
    float simTime = 10; 
    Time appsStart = Seconds(0);
    float radius = 5.0;
    float calcStart = 0; 
    bool oneDest = true;
    bool rtsCts = true;
    bool A_VO = false;
    bool VO = false; 
    bool VI = false; 
    bool A_VI = false;
    bool BE = false; 
    bool BK = false;
    double Mbps = 200;
    uint32_t seed = 1;


/* ===== Command Line parameters ===== */

    CommandLine cmd;
    cmd.AddValue("nSTA", "Number of stations", nSTA);
    cmd.AddValue("packetSize", "Packet size [B]", packetSize);
    cmd.AddValue("simTime", "simulation time [s]", simTime);
    cmd.AddValue("calcStart", "start of results analysis [s]", calcStart);
    cmd.AddValue("radius", "Radius of area [m] to randomly place stations", radius);
    cmd.AddValue("oneDest", "use one traffic destination?", oneDest);
    cmd.AddValue("RTSCTS", "use RTS/CTS?", rtsCts);
    cmd.AddValue("A_VO", "run A_VO traffic?", A_VO);
    cmd.AddValue("VO", "run VO traffic?", VO);
    cmd.AddValue("VI", "run VI traffic?", VI);
    cmd.AddValue("A_VI", "run A_VI traffic?", A_VI);
    cmd.AddValue("BE", "run BE traffic?", BE);
    cmd.AddValue("BK", "run BK traffic?", BK);
    cmd.AddValue("Mbps", "traffic generated per queue [Mbps]", Mbps);
    cmd.AddValue("seed", "Seed", seed);
    cmd.Parse(argc, argv);

  Time simulationTime = Seconds (simTime);
  ns3::RngSeedManager::SetSeed (seed);
 
  Packet::EnablePrinting ();

  NodeContainer sta;
  sta.Create (nSTA+1);



/* ======== Positioning / Mobility ======= */
  
  Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
  positionAlloc->Add (Vector (0.0, 0.0, 0.0));
    for (uint32_t i = 0; i < nSTA; i++)
        positionAlloc->Add(Vector(radius * sin(2 * M_PI * (float) i / (float) nSTA),
                                  radius * cos(2 * M_PI * (float) i / (float) nSTA), 0.0));

  MobilityHelper mobility;
  mobility.SetPositionAllocator (positionAlloc);
  mobility.SetMobilityModel ("ns3::ConstantVelocityMobilityModel");

  mobility.Install (sta);



/* ===== Propagation Model configuration ===== */

  YansWifiChannelHelper channel = YansWifiChannelHelper::Default ();



/* ===== MAC and PHY configuration ===== */

  YansWifiPhyHelper phy = YansWifiPhyHelper::Default ();
  phy.SetChannel (channel.Create ());

  WifiHelper wifi;
  WifiMacHelper mac; //802.11ac
  // wifi.SetStandard (WIFI_PHY_STANDARD_80211ac);
  wifi.SetStandard(WIFI_PHY_STANDARD_80211ac);

  //PHY parameters
  phy.Set("Antennas", UintegerValue(2)); //[1-4] for 802.11n/ac - see http://mcsindex.com/
  phy.Set("MaxSupportedTxSpatialStreams", UintegerValue(2)); //[1-4] for 802.11n/ac - see http://mcsindex.com/
  phy.Set("MaxSupportedRxSpatialStreams", UintegerValue(2)); //[1-4] for 802.11n/ac - see http://mcsindex.com/

  //MAC parameters
  //for complete list of available parameters - see Attributes on https://www.nsnam.org/doxygen/classns3_1_1_adhoc_wifi_mac.html#pri-methods
    mac.SetType("ns3::AdhocWifiMac",
                "QosSupported", BooleanValue(true),
                "Ssid", SsidValue(Ssid("TEST")),
                "BK_MaxAmsduSize", UintegerValue(11398),
                "AltEDCASupported", BooleanValue(true));


  //WiFi Remote Station Manager parameters 

  //Constant Rate setting - see Attributes on https://www.nsnam.org/doxygen/classns3_1_1_constant_rate_wifi_manager.html#pri-attribs
  // wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
	// 						    "DataMode", StringValue ("OfdmRate54Mbps") ); 
  wifi.SetRemoteStationManager("ns3::MinstrelHtWifiManager",
                                "RtsCtsThreshold", UintegerValue(rtsCts ? 0 : 2500),
                                "FragmentationThreshold", UintegerValue(2500));


  NetDeviceContainer staDevices = wifi.Install (phy, mac, sta);

  Config::Set("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Phy/ChannelWidth",
              UintegerValue(160)); //for 802.11n/ac - see http://mcsindex.com/


/* ===== Internet stack ===== */

  InternetStackHelper stack;
  stack.Install (sta);

  Ipv4AddressHelper address;

  address.SetBase ("192.168.1.0", "255.255.255.0");
  Ipv4InterfaceContainer staIf;
  staIf = address.Assign (staDevices);



/* ===== Setting applications ===== */

  DataRate dataRate = DataRate(1000000 * Mbps); // - BK 200 Mb/s
  DataRate dataRate_HD_MPEG4 = DataRate(1000000 * 6); // - high definision MPEG-4 6Mb/s

  uint32_t destinationSTANumber = nSTA; //for one common traffic destination
  Ptr<Node> dest = sta.Get (destinationSTANumber);

  //Configure CBR traffic sources

    for (uint32_t i = 0; i < nSTA; i++) {
        Ptr <Node> node = sta.Get(i);

        Ipv4Address destinationReverse = staIf.GetAddress(i);

        if (A_VO) {
            OnOffHelper onOffHelper_A_VO_rev = SimulationHelper::CreateOnOffHelper("ns3::UdpSocketFactory",
                                                                                 InetSocketAddress(destinationReverse,
                                                                                                   1007), dataRate,
                                                                                 packetSize, 7, appsStart,
                                                                                 simulationTime);
            onOffHelper_A_VO_rev.Install(dest);
            PacketSinkHelper sink_A_VO("ns3::UdpSocketFactory", InetSocketAddress(destinationReverse, 1007));
            sink_A_VO.Install(node);
        }
        if (VO) {
            OnOffHelper onOffHelper_VO_rev = SimulationHelper::CreateOnOffHelper("ns3::UdpSocketFactory",
                                                                                 InetSocketAddress(destinationReverse,
                                                                                                   1006), dataRate,
                                                                                 packetSize, 6, appsStart,
                                                                                 simulationTime);
            onOffHelper_VO_rev.Install(dest);
            PacketSinkHelper sink_VO("ns3::UdpSocketFactory", InetSocketAddress(destinationReverse, 1006));
            sink_VO.Install(node);
        }
        if (VI) {
            OnOffHelper onOffHelper_VI_rev = SimulationHelper::CreateOnOffHelper("ns3::UdpSocketFactory",
                                                                                 InetSocketAddress(destinationReverse,
                                                                                                   1005), dataRate_HD_MPEG4,
                                                                                 packetSize, 5, appsStart,
                                                                                 simulationTime);
            onOffHelper_VI_rev.Install(dest);
            PacketSinkHelper sink_VI("ns3::UdpSocketFactory", InetSocketAddress(destinationReverse, 1005));
            sink_VI.Install(node);
        }
        if (A_VI) {
            OnOffHelper onOffHelper_A_VI_rev = SimulationHelper::CreateOnOffHelper("ns3::UdpSocketFactory",
                                                                                 InetSocketAddress(destinationReverse,
                                                                                                   1004), dataRate,
                                                                                 packetSize, 4, appsStart,
                                                                                 simulationTime);
            onOffHelper_A_VI_rev.Install(dest);
            PacketSinkHelper sink_A_VI("ns3::UdpSocketFactory", InetSocketAddress(destinationReverse, 1004));
            sink_A_VI.Install(node);
        }
        if (BE) {
            OnOffHelper onOffHelper_BE_rev = SimulationHelper::CreateOnOffHelper("ns3::UdpSocketFactory",
                                                                                 InetSocketAddress(destinationReverse,
                                                                                                   1000), dataRate,
                                                                                 packetSize, 0, appsStart,
                                                                                 simulationTime);
            onOffHelper_BE_rev.Install(dest);
            PacketSinkHelper sink_BE("ns3::UdpSocketFactory", InetSocketAddress(destinationReverse, 1000));
            sink_BE.Install(node);
        }
        if (BK) {
            OnOffHelper onOffHelper_BK_rev = SimulationHelper::CreateOnOffHelper("ns3::UdpSocketFactory",
                                                                                 InetSocketAddress(destinationReverse,
                                                                                                   1001), dataRate,
                                                                                 packetSize, 1, appsStart,
                                                                                 simulationTime);
            onOffHelper_BK_rev.Install(dest);
            PacketSinkHelper sink_BK("ns3::UdpSocketFactory", InetSocketAddress(destinationReverse, 1001));
            sink_BK.Install(node);
        }
    }


/* ===== tracing configuration ====== */

  // phy.EnablePcap ("out", nSTA-1, 0); // sniffing to PCAP file

  //AsciiTraceHelper ascii;
  //phy.EnableAsciiAll (ascii.CreateFileStream ("out.tr"));
  //phy.EnableAscii (ascii.CreateFileStream ("out.tr"), sta.Get (0)->GetDevice (0));

  FlowMonitorHelper flowmon_helper;
  Ptr<FlowMonitor> monitor = flowmon_helper.InstallAll ();
  monitor->SetAttribute ("StartTime", TimeValue (Seconds (calcStart) ) ); //Time from which flowmonitor statistics are gathered.
  monitor->SetAttribute ("DelayBinWidth", DoubleValue (0.001));
  monitor->SetAttribute ("JitterBinWidth", DoubleValue (0.001));
  monitor->SetAttribute ("PacketSizeBinWidth", DoubleValue (20));



/* ===== running simulation ========= */

  SimulationHelper::PopulateArpCache ();
  Simulator::Stop (simulationTime);
  Simulator::Run ();
  Simulator::Destroy ();



/* ===== printing results ===== */

  monitor->CheckForLostPackets ();

  //monitor->SerializeToXmlFile ("out.xml", true, true); // sniffing to XML file
  
  std::string proto;
  //initialize variables for overall results calculation
  uint64_t txBytes = 0, rxBytes = 0, txPackets = 0, rxPackets = 0, lostPackets = 0;
  double throughput;
  Time delaySum = Seconds (0), jitterSum = Seconds (0);

  Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmon_helper.GetClassifier ());
  //iterate over traffic flows
  std::map< FlowId, FlowMonitor::FlowStats > stats = monitor->GetFlowStats ();
  for (std::map<FlowId, FlowMonitor::FlowStats>::iterator flow = stats.begin (); flow != stats.end (); flow++)
    {
      Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (flow->first);

      //print results for given traffic flow
      switch (t.protocol)
        {
          case (6):
            proto = "TCP";
            break;
          case (17):
            proto = "UDP";
            break;
          default:
            exit (1);
        }
      std::cout << "FlowID: " << flow->first << "(" << proto << " "
                << t.sourceAddress << "/" << t.sourcePort << " --> "
                << t.destinationAddress << "/" << t.destinationPort << ")" <<
      std::endl;

      std::cout << "  Tx bytes:\t"     << flow->second.txBytes << std::endl;
      std::cout << "  Rx bytes:\t"     << flow->second.rxBytes << std::endl;
      std::cout << "  Tx packets:\t"   << flow->second.txPackets << std::endl;
      std::cout << "  Rx packets:\t"   << flow->second.rxPackets << std::endl;
      std::cout << "  Lost packets:\t" << flow->second.lostPackets << std::endl;
      if (flow->second.rxPackets > 0)
        {
          //std::cout << "  Throughput:\t"   << flow->second.rxBytes * 8.0 / (flow->second.timeLastRxPacket.GetSeconds ()-flow->second.timeFirstTxPacket.GetSeconds ()) / 1000000  << " Mb/s" << std::endl;
          std::cout << "  Throughput:\t"   << flow->second.rxBytes * 8.0 / (simulationTime - Seconds (calcStart)).GetMicroSeconds ()  << " Mb/s" << std::endl;
          std::cout << "  Mean delay:\t"   << (double)(flow->second.delaySum / (flow->second.rxPackets)).GetMicroSeconds () / 1000 << " ms" << std::endl;    
          if (flow->second.rxPackets > 1)
            std::cout << "  Mean jitter:\t"  << (double)(flow->second.jitterSum / (flow->second.rxPackets - 1)).GetMicroSeconds () / 1000 << " ms" << std::endl;   
          else
            std::cout << "  Mean jitter:\t---"   << std::endl;
        }
      else
        {
          std::cout << "  Throughput:\t0 Mb/s" << std::endl;
          std::cout << "  Mean delay:\t---"    << std::endl;    
          std::cout << "  Mean jitter:\t---"   << std::endl;
        }

      //increase variables for overall results calculation
      txBytes     += flow->second.txBytes;
      rxBytes     += flow->second.rxBytes;
      txPackets   += flow->second.txPackets;
      rxPackets   += flow->second.rxPackets;
      lostPackets += flow->second.lostPackets;
      //throughput  += (flow->second.rxPackets > 0 ? flow->second.rxBytes * 8.0 / (flow->second.timeLastRxPacket.GetSeconds ()-flow->second.timeFirstTxPacket.GetSeconds ()) / 1000000 : 0);
      throughput  += (flow->second.rxPackets > 0 ? flow->second.rxBytes * 8.0 / (simulationTime - Seconds (calcStart)).GetMicroSeconds () : 0);
      delaySum    += flow->second.delaySum;
      jitterSum   += flow->second.jitterSum;
    }


  //print overall results
  std::cout << "=======================Total: =====================================" << std::endl;

  std::cout << "  Tx bytes:\t"     << txBytes     << std::endl;
  std::cout << "  Rx bytes:\t"     << rxBytes     << std::endl;
  std::cout << "  Tx packets:\t"   << txPackets   << std::endl;
  std::cout << "  Rx packets:\t"   << rxPackets   << std::endl;
  std::cout << "  Lost packets:\t" << lostPackets << std::endl;
  std::cout << "  Throughput:\t"   << throughput  << " Mb/s" << std::endl;
  if (rxPackets > 0)
    {
      std::cout << "  Mean delay:\t"   << (double)(delaySum / (rxPackets)).GetMicroSeconds () / 1000 << " ms" << std::endl;    
      if (rxPackets > 1)  
        std::cout << "  Mean jitter:\t"  << (double)(jitterSum / (rxPackets - 1)).GetMicroSeconds () / 1000  << " ms" << std::endl;   
      else
        std::cout << "  Mean jitter:\t---"   << std::endl;
    }
  else
    {
      std::cout << "  Mean delay:\t---"    << std::endl;    
      std::cout << "  Mean jitter:\t---"   << std::endl;
    }


  return 0;
}
