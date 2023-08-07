
INCLUDEPATH += $$PWD

LIBS += /home/clever/libnetsnmp.so.40



HEADERS += \
    $$PWD/snmpthread.h \
    $$PWD/net-snmp/agent/agent_callbacks.h \
    $$PWD/net-snmp/agent/agent_handler.h \
    $$PWD/net-snmp/agent/agent_index.h \
    $$PWD/net-snmp/agent/agent_read_config.h \
    $$PWD/net-snmp/agent/agent_registry.h \
    $$PWD/net-snmp/agent/agent_sysORTable.h \
    $$PWD/net-snmp/agent/agent_trap.h \
    $$PWD/net-snmp/agent/all_helpers.h \
    $$PWD/net-snmp/agent/auto_nlist.h \
    $$PWD/net-snmp/agent/baby_steps.h \
    $$PWD/net-snmp/agent/bulk_to_next.h \
    $$PWD/net-snmp/agent/cache_handler.h \
    $$PWD/net-snmp/agent/debug_handler.h \
    $$PWD/net-snmp/agent/ds_agent.h \
    $$PWD/net-snmp/agent/hardware/cpu.h \
    $$PWD/net-snmp/agent/hardware/fsys.h \
    $$PWD/net-snmp/agent/hardware/memory.h \
    $$PWD/net-snmp/agent/hardware/sensors.h \
    $$PWD/net-snmp/agent/instance.h \
    $$PWD/net-snmp/agent/mfd.h \
    $$PWD/net-snmp/agent/mib_modules.h \
    $$PWD/net-snmp/agent/mode_end_call.h \
    $$PWD/net-snmp/agent/multiplexer.h \
    $$PWD/net-snmp/agent/net-snmp-agent-includes.h \
    $$PWD/net-snmp/agent/netsnmp_close_fds.h \
    $$PWD/net-snmp/agent/null.h \
    $$PWD/net-snmp/agent/object_monitor.h \
    $$PWD/net-snmp/agent/old_api.h \
    $$PWD/net-snmp/agent/read_only.h \
    $$PWD/net-snmp/agent/row_merge.h \
    $$PWD/net-snmp/agent/scalar.h \
    $$PWD/net-snmp/agent/scalar_group.h \
    $$PWD/net-snmp/agent/serialize.h \
    $$PWD/net-snmp/agent/set_helper.h \
    $$PWD/net-snmp/agent/snmp_agent.h \
    $$PWD/net-snmp/agent/snmp_get_statistic.h \
    $$PWD/net-snmp/agent/snmp_vars.h \
    $$PWD/net-snmp/agent/stash_cache.h \
    $$PWD/net-snmp/agent/stash_to_next.h \
    $$PWD/net-snmp/agent/sysORTable.h \
    $$PWD/net-snmp/agent/table.h \
    $$PWD/net-snmp/agent/table_array.h \
    $$PWD/net-snmp/agent/table_container.h \
    $$PWD/net-snmp/agent/table_data.h \
    $$PWD/net-snmp/agent/table_dataset.h \
    $$PWD/net-snmp/agent/table_iterator.h \
    $$PWD/net-snmp/agent/table_tdata.h \
    $$PWD/net-snmp/agent/var_struct.h \
    $$PWD/net-snmp/agent/watcher.h \
    $$PWD/net-snmp/config_api.h \
    $$PWD/net-snmp/data_access/arp.h \
    $$PWD/net-snmp/data_access/defaultrouter.h \
    $$PWD/net-snmp/data_access/interface.h \
    $$PWD/net-snmp/data_access/ip_scalars.h \
    $$PWD/net-snmp/data_access/ipaddress.h \
    $$PWD/net-snmp/data_access/ipstats.h \
    $$PWD/net-snmp/data_access/net-snmp-data-access-includes.h \
    $$PWD/net-snmp/data_access/route.h \
    $$PWD/net-snmp/data_access/scopezone.h \
    $$PWD/net-snmp/data_access/swinst.h \
    $$PWD/net-snmp/data_access/swrun.h \
    $$PWD/net-snmp/data_access/systemstats.h \
    $$PWD/net-snmp/data_access/tcpConn.h \
    $$PWD/net-snmp/data_access/udp_endpoint.h \
    $$PWD/net-snmp/definitions.h \
    $$PWD/net-snmp/library/asn1.h \
    $$PWD/net-snmp/library/callback.h \
    $$PWD/net-snmp/library/cert_util.h \
    $$PWD/net-snmp/library/check_varbind.h \
    $$PWD/net-snmp/library/container.h \
    $$PWD/net-snmp/library/container_binary_array.h \
    $$PWD/net-snmp/library/container_iterator.h \
    $$PWD/net-snmp/library/container_list_ssll.h \
    $$PWD/net-snmp/library/container_null.h \
    $$PWD/net-snmp/library/data_list.h \
    $$PWD/net-snmp/library/default_store.h \
    $$PWD/net-snmp/library/dir_utils.h \
    $$PWD/net-snmp/library/fd_event_manager.h \
    $$PWD/net-snmp/library/file_utils.h \
    $$PWD/net-snmp/library/getopt.h \
    $$PWD/net-snmp/library/int64.h \
    $$PWD/net-snmp/library/keytools.h \
    $$PWD/net-snmp/library/large_fd_set.h \
    $$PWD/net-snmp/library/lcd_time.h \
    $$PWD/net-snmp/library/libsnmp.h \
    $$PWD/net-snmp/library/md5.h \
    $$PWD/net-snmp/library/mib.h \
    $$PWD/net-snmp/library/mt_support.h \
    $$PWD/net-snmp/library/netsnmp-attribute-format.h \
    $$PWD/net-snmp/library/oid.h \
    $$PWD/net-snmp/library/oid_stash.h \
    $$PWD/net-snmp/library/openssl_aes.h \
    $$PWD/net-snmp/library/openssl_des.h \
    $$PWD/net-snmp/library/openssl_md5.h \
    $$PWD/net-snmp/library/openssl_sha.h \
    $$PWD/net-snmp/library/parse.h \
    $$PWD/net-snmp/library/read_config.h \
    $$PWD/net-snmp/library/scapi.h \
    $$PWD/net-snmp/library/sd-daemon.h \
    $$PWD/net-snmp/library/snmp-tc.h \
    $$PWD/net-snmp/library/snmp.h \
    $$PWD/net-snmp/library/snmpAAL5PVCDomain.h \
    $$PWD/net-snmp/library/snmpAliasDomain.h \
    $$PWD/net-snmp/library/snmpCallbackDomain.h \
    $$PWD/net-snmp/library/snmpDTLSUDPDomain.h \
    $$PWD/net-snmp/library/snmpIPXDomain.h \
    $$PWD/net-snmp/library/snmpIPv4BaseDomain.h \
    $$PWD/net-snmp/library/snmpIPv6BaseDomain.h \
    $$PWD/net-snmp/library/snmpSSHDomain.h \
    $$PWD/net-snmp/library/snmpSTDDomain.h \
    $$PWD/net-snmp/library/snmpSocketBaseDomain.h \
    $$PWD/net-snmp/library/snmpTCPBaseDomain.h \
    $$PWD/net-snmp/library/snmpTCPDomain.h \
    $$PWD/net-snmp/library/snmpTCPIPv6Domain.h \
    $$PWD/net-snmp/library/snmpTLSBaseDomain.h \
    $$PWD/net-snmp/library/snmpTLSTCPDomain.h \
    $$PWD/net-snmp/library/snmpUDPBaseDomain.h \
    $$PWD/net-snmp/library/snmpUDPDomain.h \
    $$PWD/net-snmp/library/snmpUDPIPv4BaseDomain.h \
    $$PWD/net-snmp/library/snmpUDPIPv6Domain.h \
    $$PWD/net-snmp/library/snmpUDPsharedDomain.h \
    $$PWD/net-snmp/library/snmpUnixDomain.h \
    $$PWD/net-snmp/library/snmp_alarm.h \
    $$PWD/net-snmp/library/snmp_api.h \
    $$PWD/net-snmp/library/snmp_assert.h \
    $$PWD/net-snmp/library/snmp_client.h \
    $$PWD/net-snmp/library/snmp_debug.h \
    $$PWD/net-snmp/library/snmp_enum.h \
    $$PWD/net-snmp/library/snmp_impl.h \
    $$PWD/net-snmp/library/snmp_logging.h \
    $$PWD/net-snmp/library/snmp_openssl.h \
    $$PWD/net-snmp/library/snmp_parse_args.h \
    $$PWD/net-snmp/library/snmp_secmod.h \
    $$PWD/net-snmp/library/snmp_service.h \
    $$PWD/net-snmp/library/snmp_transport.h \
    $$PWD/net-snmp/library/snmpksm.h \
    $$PWD/net-snmp/library/snmptsm.h \
    $$PWD/net-snmp/library/snmpusm.h \
    $$PWD/net-snmp/library/snmpv3.h \
    $$PWD/net-snmp/library/system.h \
    $$PWD/net-snmp/library/testing.h \
    $$PWD/net-snmp/library/text_utils.h \
    $$PWD/net-snmp/library/tools.h \
    $$PWD/net-snmp/library/transform_oids.h \
    $$PWD/net-snmp/library/types.h \
    $$PWD/net-snmp/library/ucd_compat.h \
    $$PWD/net-snmp/library/vacm.h \
    $$PWD/net-snmp/library/winpipe.h \
    $$PWD/net-snmp/library/winservice.h \
    $$PWD/net-snmp/machine/generic.h \
    $$PWD/net-snmp/mib_api.h \
    $$PWD/net-snmp/net-snmp-config.h \
    $$PWD/net-snmp/net-snmp-features.h \
    $$PWD/net-snmp/net-snmp-includes.h \
    $$PWD/net-snmp/openssl_md5.h \
    $$PWD/net-snmp/output_api.h \
    $$PWD/net-snmp/pdu_api.h \
    $$PWD/net-snmp/session_api.h \
    $$PWD/net-snmp/snmpv3_api.h \
    $$PWD/net-snmp/system/aix.h \
    $$PWD/net-snmp/system/bsd.h \
    $$PWD/net-snmp/system/bsdi.h \
    $$PWD/net-snmp/system/bsdi3.h \
    $$PWD/net-snmp/system/bsdi4.h \
    $$PWD/net-snmp/system/cygwin.h \
    $$PWD/net-snmp/system/darwin.h \
    $$PWD/net-snmp/system/dragonfly.h \
    $$PWD/net-snmp/system/dynix.h \
    $$PWD/net-snmp/system/freebsd.h \
    $$PWD/net-snmp/system/freebsd10.h \
    $$PWD/net-snmp/system/freebsd11.h \
    $$PWD/net-snmp/system/freebsd12.h \
    $$PWD/net-snmp/system/freebsd13.h \
    $$PWD/net-snmp/system/freebsd14.h \
    $$PWD/net-snmp/system/freebsd2.h \
    $$PWD/net-snmp/system/freebsd3.h \
    $$PWD/net-snmp/system/freebsd4.h \
    $$PWD/net-snmp/system/freebsd5.h \
    $$PWD/net-snmp/system/freebsd6.h \
    $$PWD/net-snmp/system/freebsd7.h \
    $$PWD/net-snmp/system/freebsd8.h \
    $$PWD/net-snmp/system/freebsd9.h \
    $$PWD/net-snmp/system/generic.h \
    $$PWD/net-snmp/system/hpux.h \
    $$PWD/net-snmp/system/irix.h \
    $$PWD/net-snmp/system/kfreebsd.h \
    $$PWD/net-snmp/system/linux.h \
    $$PWD/net-snmp/system/mingw32.h \
    $$PWD/net-snmp/system/mingw32msvc.h \
    $$PWD/net-snmp/system/mips.h \
    $$PWD/net-snmp/system/netbsd.h \
    $$PWD/net-snmp/system/nto-qnx6.h \
    $$PWD/net-snmp/system/openbsd.h \
    $$PWD/net-snmp/system/openbsd4.h \
    $$PWD/net-snmp/system/openbsd5.h \
    $$PWD/net-snmp/system/openbsd6.h \
    $$PWD/net-snmp/system/openbsd7.h \
    $$PWD/net-snmp/system/osf5.h \
    $$PWD/net-snmp/system/solaris.h \
    $$PWD/net-snmp/system/solaris2.3.h \
    $$PWD/net-snmp/system/solaris2.4.h \
    $$PWD/net-snmp/system/solaris2.5.h \
    $$PWD/net-snmp/system/solaris2.6.h \
    $$PWD/net-snmp/system/sunos.h \
    $$PWD/net-snmp/system/svr5.h \
    $$PWD/net-snmp/system/sysv.h \
    $$PWD/net-snmp/system/ultrix4.h \
    $$PWD/net-snmp/types.h \
    $$PWD/net-snmp/utilities.h \
    $$PWD/net-snmp/varbind_api.h \
    $$PWD/net-snmp/version.h

    
   
	
SOURCES += \
    $$PWD/snmpthread.cpp
    
    

