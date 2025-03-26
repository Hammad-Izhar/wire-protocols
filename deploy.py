import subprocess
import argparse
import yaml
import json
import os

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Spin up SocketOut servers according to a YAML configuration file.")
    parser.add_argument(
        "config",
        type=str,
        help="Path to the YAML configuration file for SocketOut servers.",
    )

    args = parser.parse_args()
    config_path = args.config
    config_dir = os.path.dirname(config_path)
    # Load the YAML configuration file
    with open(config_path, "r") as file:
        config = yaml.safe_load(file)

    # Create the config file for each replica and load balancer
    for module in [*(config["replica"] or []), *(config["load_balancer"] or [])]:
        module_config = json.dumps(module)
        with open(os.path.join(config_dir, f"{module['name']}_config.json"), "w") as config_file:
            config_file.write(module_config)
        print(f"Created config for replica {module['name']}")

        if module.get("db") is not None:
            os.makedirs(module["db"], exist_ok=True)


    subprocess.run(["tmux", "new-session", "-d", "-s", "socketout"])

    # tmux make a window for replicas and a window for load balancers
    subprocess.run(["tmux", "rename-window", "-t", "socketout:0", "replicas"])
    subprocess.run(["tmux", "new-window", "-t", "socketout:1", "-n", "load_balancers"])


    if config["replica"] and len(config["replica"]) > 0:
        subprocess.run([
            "tmux", "send-keys", "-t", "socketout:replicas.0",
            f"build/server_rpc --config {os.path.join(config_dir, f'{config['replica'][0]['name']}_config.json')}"
        ])

        if len(config["replica"]) > 1:
            for replica in config["replica"][1:]:
                # Split the active pane in the "replicas" window horizontally.
                subprocess.run(["tmux", "split-window", "-h", "-t", "socketout:replicas"])
                # Send the command to the new (active) pane.
                subprocess.run([
                    "tmux", "send-keys", "-t", "socketout:replicas",
                    f"python3 socketout.py {replica['name']}_config.json"
                ])
                # Rearrange the layout to keep it tiled.
                subprocess.run(["tmux", "select-layout", "-t", "socketout:replicas", "tiled"])

    if config["load_balancer"] and len(config["load_balancer"]) > 0:
        subprocess.run([
            "tmux", "send-keys", "-t", "socketout:load_balancers.0",
            f"python3 socketout.py {config['load_balancer'][0]['name']}_config.json"
        ])
        if len(config["load_balancer"]) > 1:
            for load_balancer in config["load_balancer"][1:]:
                subprocess.run(["tmux", "split-window", "-h", "-t", "socketout:load_balancers"])
                subprocess.run([
                    "tmux", "send-keys", "-t", "socketout:load_balancers",
                    f"python3 socketout.py {load_balancer['name']}_config.json"
                ])
                subprocess.run(["tmux", "select-layout", "-t", "socketout:load_balancers", "tiled"])


    subprocess.run(["tmux", "set", "-g", "mouse", "on"])
    subprocess.run(["tmux", "select-window", "-t", "socketout:replicas"])
    subprocess.run(["tmux", "attach", "-t", "socketout"])