(* virt-sysprep
 * Copyright (C) 2012 Red Hat Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *)

open Printf

open Utils
open Sysprep_operation
open Sysprep_gettext.Gettext

module G = Guestfs

let hostname = ref "localhost.localdomain"

let hostname_perform g root =
  let typ = g#inspect_get_type root in
  let distro = g#inspect_get_distro root in
  let major_version = g#inspect_get_major_version root in

  let update_etc_hostname () =
    g#write "/etc/hostname" !hostname;
    [ `Created_files ]
  in

  match typ, distro, major_version with
    (* Fedora 18 (hence RHEL 7+) changed to using /etc/hostname
     * (RHBZ#881953, RHBZ#858696).
     *)
  | "linux", "fedora", v when v >= 18 -> update_etc_hostname ()
  | "linux", "rhel", v when v >= 7 -> update_etc_hostname ()
  | "linux", ("debian"|"ubuntu"), _ -> update_etc_hostname ()

  | "linux", ("fedora"|"rhel"|"centos"|"scientificlinux"|"redhat-based"), _ ->
    let filename = "/etc/sysconfig/network" in
    if g#is_file filename then (
      (* Replace HOSTNAME=... entry.  The code assumes it's a small,
       * plain text file.
       *)
      let lines = Array.to_list (g#read_lines filename) in
      let lines = List.filter (
        fun line -> not (string_prefix line "HOSTNAME=")
      ) lines in
      let file =
        String.concat "\n" lines ^
          sprintf "\nHOSTNAME=%s\n" !hostname in
      g#write filename file;
    ) else (
      let file = sprintf "HOSTNAME=%s\n" !hostname in
      g#write filename file;
    );
    [ `Created_files ]

  | "linux", ("opensuse"|"sles"|"suse-based"), _ ->
    g#write "/etc/HOSTNAME" !hostname;
    [ `Created_files ]

  | _ -> []

let hostname_op = {
  name = "hostname";
  enabled_by_default = true;
  heading = s_"Change the hostname of the guest";
  pod_description = Some (s_"\
This operation changes the hostname of the guest to the value
given in the I<--hostname> parameter.

If the I<--hostname> parameter is not given, then the hostname is changed
to C<localhost.localdomain>.");
  extra_args = [
    ("--hostname", Arg.Set_string hostname, s_"hostname" ^ " " ^ s_"New hostname"),
    s_"\
Change the hostname.  If not given, defaults to C<localhost.localdomain>."
  ];
  perform_on_filesystems = Some hostname_perform;
  perform_on_devices = None;
}

let () = register_operation hostname_op
