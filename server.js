const WebSocket = require("ws");
const { mouse, Point } = require("@nut-tree-fork/nut-js");

// Optionally, set the mouse speed for nut.js
mouse.config.mouseSpeed = 500;

const server = new WebSocket.Server({ port: 8080 });

server.on("connection", (socket) => {
  console.log("Client connected");

  socket.on("message", async (message) => {
    try {
      const data = JSON.parse(message);
      const { x, y } = data;

      // Get current mouse position
      const currentPos = await mouse.getPosition();

      // Calculate new mouse position
      const newX = currentPos.x + x * 10; // Scale motion data
      const newY = currentPos.y - y * 10;

      // Move the mouse
      await mouse.setPosition(new Point(newX, newY));
    } catch (err) {
      console.error("Error processing message:", err);
    }
  });

  socket.on("close", () => console.log("Client disconnected"));
});

console.log("WebSocket server running on ws://localhost:8080");
