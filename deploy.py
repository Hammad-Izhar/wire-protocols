import subprocess
import argparse
import yaml
import json
import os
import socket

def get_local_ipv4():
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    try:
        # The IP doesn't have to be reachable; we're just using it to determine the local IP
        s.connect(('8.8.8.8', 80))
        ip = s.getsockname()[0]
    except Exception:
        ip = '127.0.0.1'
    finally:
        s.close()
    return ip



if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Spin up SocketOut servers according to a YAML configuration file.")
    parser.add_argument(
        "config",
        type=str,
        help="Path to the YAML configuration file for SocketOut servers.",
    )
    parser.add_argument(
        "--clean",
        action="store_true",
        help="Make new db"
    )

    args = parser.parse_args()
    config_path = args.config
    config_dir = os.path.dirname(config_path)

    # Load the YAML configuration file
    with open(config_path, "r") as file:
        config = yaml.safe_load(file)

    # Create the config file for each replica and load balancer
    name_to_port = {}
    for module in [*(config["replica"] or []), *(config["load_balancer"] or [])]:
        module["hostname"] = get_local_ipv4()
        name_to_port[module["name"]] = module["port"]
        module_config = json.dumps(module)
        with open(os.path.join(config_dir, f"{module['name']}_config.json"), "w") as config_file:
            config_file.write(module_config)
        print(f"Created config for replica {module['name']}")

        if module.get("db") is not None:
            if args.clean and os.path.exists(module["db"]):
                    subprocess.run(["rm", "-rf", module["db"]])
            os.makedirs(module["db"], exist_ok=True)


    subprocess.run(["tmux", "new-session", "-d", "-s", "socketout"])

    # tmux make a window for replicas and a window for load balancers
    subprocess.run(["tmux", "rename-window", "-t", "socketout:0", "replicas"])


    name_to_split = {}
    if config["replica"] and len(config["replica"]) > 0:
        subprocess.run([
            "tmux", "send-keys", "-t", "socketout:replicas.0",
            f"build/server_rpc --config {os.path.join(config_dir, f'{config['replica'][0]['name']}_config.json')}", "C-m"
        ])
        name_to_split[config["replica"][0]["name"]] = 0

        if len(config["replica"]) > 1:
            for i, replica in enumerate(config["replica"][1:]):
                # Split the active pane in the "replicas" window horizontally.
                subprocess.run(["tmux", "split-window", "-h", "-t", "socketout:replicas"])
                name_to_split[replica["name"]] = i + 1
                # Send the command to the new (active) pane.
                subprocess.run([
                    "tmux", "send-keys", "-t", "socketout:replicas",
                      f"build/server_rpc --config {os.path.join(config_dir, f'{replica['name']}_config.json')}", "C-m"
                ])
                # Rearrange the layout to keep it tiled.
                subprocess.run(["tmux", "select-layout", "-t", "socketout:replicas", "tiled"])

    for [replica_1, replica_2] in config["topology"]:
        # write the command f"a localhost ${name_to_port[replica_2]}" in the terminal split of the first replica
        subprocess.run([
            "tmux", "send-keys", "-t", f"socketout:replicas.{name_to_split[replica_1]}",
            f"a {get_local_ipv4()} {name_to_port[replica_2]}", "C-m"
        ])

    subprocess.run(["tmux", "set", "-g", "mouse", "on"])
    subprocess.run(["tmux", "select-window", "-t", "socketout:replicas"])
    subprocess.run(["tmux", "attach", "-t", "socketout"])